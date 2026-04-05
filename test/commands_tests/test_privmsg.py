import socket
import time
import unittest

SERVER = "127.0.0.1"
PORT = 6667
PASSWORD = "testpass" # Doit correspondre au mdp de ton serveur

class TestPrivmsg(unittest.TestCase):

    def setUp(self):
        """Initialisation optionnelle avant chaque test"""
        pass

    def create_registered_client(self, nickname, username):
        """Méthode utilitaire pour connecter et enregistrer un client"""
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((SERVER, PORT))
        sock.sendall(f"PASS {PASSWORD}\r\n".encode())
        sock.sendall(f"NICK {nickname}\r\n".encode())
        sock.sendall(f"USER {username} 0 * :Realname\r\n".encode())
        time.sleep(0.1)
        sock.recv(4096) # Vide le message de bienvenue (001)
        return sock

    def test_privmsg_success(self):
        """Vérifie l'envoi réussi d'un message entre deux clients"""
        alice = self.create_registered_client("Alice", "alice")
        bob = self.create_registered_client("Bob", "bob")

        # Alice envoie à Bob
        alice.sendall(b"PRIVMSG Bob :Hello Bob!\r\n")
        time.sleep(0.1)

        # Bob reçoit
        response = bob.recv(4096).decode()
        alice.close()
        bob.close()

        self.assertIn(":Alice!alice", response)
        self.assertIn("PRIVMSG Bob :Hello Bob!", response)

    def test_err_nosuchnick(self):
        """Vérifie l'erreur 401 si le destinataire n'existe pas"""
        alice = self.create_registered_client("Alice", "alice")
        alice.sendall(b"PRIVMSG Inconnu :Tu es la ?\r\n")
        time.sleep(0.1)
        
        response = alice.recv(4096).decode()
        alice.close()
        
        self.assertIn("401", response, "Devrait renvoyer ERR_NOSUCHNICK (401)")

    def test_err_notexttosend(self):
        """Vérifie l'erreur 412 si le texte est vide"""
        alice = self.create_registered_client("Alice", "alice")
        alice.sendall(b"PRIVMSG Alice :\r\n") # Texte vide
        time.sleep(0.1)
        
        response = alice.recv(4096).decode()
        alice.close()
        
        self.assertIn("412", response, "Devrait renvoyer ERR_NOTEXTTOSEND (412)")
    
    def test_err_nosuchchannel(self):
        """Vérifie l'erreur 403 si on cible un canal (temporairement bloqué)"""
        alice = self.create_registered_client("Alice", "alice")
        
        alice.sendall(b"PRIVMSG #general :Hello le channel!\r\n")
        time.sleep(0.1)
        
        response = alice.recv(4096).decode()
        alice.close()
        
        self.assertIn("403", response, "Devrait renvoyer ERR_NOSUCHCHANNEL (403) pour un #")

    def test_err_norecipient(self):
        """Vérifie l'erreur 411 si aucun destinataire n'est précisé"""
        alice = self.create_registered_client("Alice", "alice")
        
        # Envoi de la commande avec un trailing text mais pas de cible
        alice.sendall(b"PRIVMSG :Message sans cible\r\n")
        time.sleep(0.1)
        
        response = alice.recv(4096).decode()
        alice.close()
        
        self.assertIn("411", response, "Devrait renvoyer ERR_NORECIPIENT (411)")

    def test_message_too_long(self):
        """Vérifie que le serveur ne crash pas sur un message > 512 octets"""
        alice = self.create_registered_client("Alice", "alice")
        bob = self.create_registered_client("Bob", "bob")
        
        # Construction d'un message qui dépasse volontairement la limite de la RFC
        base_cmd = "PRIVMSG Bob :"
        long_text = "A" * 500 # 500 + 13 + 2 (\r\n) = 515 octets
        payload = f"{base_cmd}{long_text}\r\n".encode()
        
        try:
            alice.sendall(payload)
            time.sleep(0.1)
            
            bob.sendall(b"PING :test\r\n")
            time.sleep(0.1)
            response = bob.recv(4096).decode()
            
            self.assertTrue(len(response) > 0, "Le serveur a cessé de répondre (Crash potentiel !)")
            
        finally:
            alice.close()
            bob.close()

if __name__ == '__main__':
    unittest.main()