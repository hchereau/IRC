import socket
import time
import unittest

SERVER = "127.0.0.1"
PORT = 6667
PASSWORD = "testpass" 

class TestPrivmsg(unittest.TestCase):

    def create_registered_client(self, nickname, username):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(2.0) # <-- L'ANTIDOTE CONTRE LES BOUCLES INFINIES
        sock.connect((SERVER, PORT))
        sock.sendall(f"PASS {PASSWORD}\r\n".encode())
        sock.sendall(f"NICK {nickname}\r\n".encode())
        sock.sendall(f"USER {username} 0 * :Realname\r\n".encode())
        time.sleep(0.1)
        try:
            sock.recv(4096)
        except socket.timeout:
            pass
        return sock

    def test_privmsg_success(self):
        alice = self.create_registered_client("Alice", "alice")
        bob = self.create_registered_client("Bob", "bob")

        alice.sendall(b"PRIVMSG Bob :Hello Bob!\r\n")
        time.sleep(0.1)

        try:
            response = bob.recv(4096).decode()
        except socket.timeout:
            self.fail("Timeout: Bob n'a rien reçu")
            
        alice.close()
        bob.close()

        self.assertIn(":Alice!alice", response)
        self.assertIn("PRIVMSG Bob :Hello Bob!", response)

    def test_err_nosuchnick(self):
        alice = self.create_registered_client("Alice", "alice")
        alice.sendall(b"PRIVMSG Inconnu :Tu es la ?\r\n")
        time.sleep(0.1)
        
        try:
            response = alice.recv(4096).decode()
        except socket.timeout:
            self.fail("Timeout: Le serveur n'a pas répondu à l'erreur")
            
        alice.close()
        
        # On affiche ce que le serveur a vraiment renvoyé si ça rate !
        self.assertIn("401", response, f"Échec : le serveur a renvoyé : '{response}'")

    def test_err_notexttosend(self):
        alice = self.create_registered_client("Alice", "alice")
        alice.sendall(b"PRIVMSG Alice :\r\n") 
        time.sleep(0.1)
        
        try:
            response = alice.recv(4096).decode()
        except socket.timeout:
            self.fail("Timeout")
            
        alice.close()
        self.assertIn("412", response)
    
    def test_privmsg_channel(self):
        alice = self.create_registered_client("Alice", "alice")
        bob = self.create_registered_client("Bob", "bob")
        
        alice.sendall(b"JOIN #test\r\n")
        time.sleep(0.1)
        alice.recv(4096) 
        
        bob.sendall(b"JOIN #test\r\n")
        time.sleep(0.1)
        bob.recv(4096) 
        alice.recv(4096)
        
        alice.sendall(b"PRIVMSG #test :Salut tout le monde !\r\n")
        time.sleep(0.1)
        
        try:
            response_bob = bob.recv(4096).decode()
        except socket.timeout:
            self.fail("Timeout: Bob n'a pas reçu le message du canal ! Bug dans broadcast ?")
            
        alice.close()
        bob.close()
        
        self.assertIn("PRIVMSG #test :Salut tout le monde !", response_bob)
        self.assertIn(":Alice!alice", response_bob)

    def test_err_norecipient(self):
        alice = self.create_registered_client("Alice", "alice")
        alice.sendall(b"PRIVMSG :Message sans cible\r\n")
        time.sleep(0.1)
        
        try:
            response = alice.recv(4096).decode()
        except socket.timeout:
            self.fail("Timeout")
            
        alice.close()
        self.assertIn("411", response)

    def test_message_too_long(self):
        alice = self.create_registered_client("Alice", "alice")
        bob = self.create_registered_client("Bob", "bob")
        
        base_cmd = "PRIVMSG Bob :"
        long_text = "A" * 500 
        payload = f"{base_cmd}{long_text}\r\n".encode()
        
        try:
            alice.sendall(payload)
            time.sleep(0.1)
            bob.sendall(b"PING :test\r\n")
            time.sleep(0.1)
            response = bob.recv(4096).decode()
            self.assertTrue(len(response) > 0)
        finally:
            alice.close()
            bob.close()
    
    def test_err_cannotsendtochan(self):
        """Vérifie l'erreur 404 si on envoie un message à un canal sans y être"""
        alice = self.create_registered_client("Alice", "alice")
        bob = self.create_registered_client("Bob", "bob")
        
        # Bob crée et rejoint le canal. Alice reste à l'extérieur.
        bob.sendall(b"JOIN #secret\r\n")
        time.sleep(0.1)
        bob.recv(4096)
        
        # Alice essaie d'envoyer un message au canal de Bob
        alice.sendall(b"PRIVMSG #secret :Vous me cachez quoi ?\r\n")
        time.sleep(0.1)
        
        try:
            response = alice.recv(4096).decode()
        except socket.timeout:
            self.fail("Timeout: Le serveur n'a pas répondu avec l'erreur 404")
            
        alice.close()
        bob.close()
        
        self.assertIn("404", response, f"Doit renvoyer ERR_CANNOTSENDTOCHAN (404), reçu: {response}")

if __name__ == '__main__':
    unittest.main()