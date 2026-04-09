import unittest
import socket
import time

SERVER = '127.0.0.1'
PORT = 6667

class TestNickCommand(unittest.TestCase):

    def connect_and_register(self, nickname):
        """Helper pour connecter et enregistrer rapidement un client"""
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.connect((SERVER, PORT))
        # Enregistrement complet
        client.sendall(f"PASS testpass\r\nNICK {nickname}\r\nUSER {nickname} 0 * :Real Name\r\n".encode("utf-8"))
        time.sleep(0.1)
        client.recv(4096)  # Purge des messages de bienvenue (RPL 001 à 005, etc.)
        return client

    # ==========================================
    # TESTS D'ERREURS (Basic)
    # ==========================================

    def test_nick_missing_param_in_session(self):
        """Vérifie l'erreur 431 si on tape /nick sans argument en pleine session"""
        client = self.connect_and_register("NoArgUser")
        
        client.sendall(b"NICK\r\n")
        time.sleep(0.1)
        response = client.recv(1024).decode("utf-8")
        client.close()
        
        self.assertIn("431", response, "Le serveur doit renvoyer ERR_NONICKNAMEGIVEN (431)")

    def test_nick_invalid_characters_in_session(self):
        """Vérifie l'erreur 432 si le nouveau pseudo contient des caractères interdits"""
        client = self.connect_and_register("ValidUser")
        
        client.sendall(b"NICK #Hack!er\r\n")
        time.sleep(0.1)
        response = client.recv(1024).decode("utf-8")
        client.close()
        
        self.assertIn("432", response, "Le serveur doit renvoyer ERR_ERRONEUSNICKNAME (432)")

    def test_nick_already_in_use_in_session(self):
        """Vérifie l'erreur 433 si on essaie de voler le pseudo de quelqu'un de connecté"""
        # 1. Alice se connecte
        alice = self.connect_and_register("Alice")
        
        # 2. Bob se connecte et essaie de devenir Alice
        bob = self.connect_and_register("Bob")
        bob.sendall(b"NICK Alice\r\n")
        time.sleep(0.1)
        
        response = bob.recv(1024).decode("utf-8")
        
        alice.close()
        bob.close()
        
        self.assertIn("433", response, "Le serveur doit renvoyer ERR_NICKNAMEINUSE (433)")

    # ==========================================
    # TESTS DE SUCCÈS & BROADCAST (Tricky)
    # ==========================================

    def test_nick_basic_change_success(self):
        """Vérifie qu'un changement de pseudo valide est bien pris en compte et notifié au client"""
        client = self.connect_and_register("OldNick")
        
        client.sendall(b"NICK NewNick\r\n")
        time.sleep(0.1)
        response = client.recv(1024).decode("utf-8")
        client.close()
        
        # Le serveur doit renvoyer au client : :OldNick!user@host NICK :NewNick
        self.assertIn("NICK :NewNick", response, "Le client ne s'est pas vu notifier son propre changement de pseudo")
        self.assertTrue(response.startswith(":OldNick"), "La notification ne commence pas par l'ancien préfixe")

    def test_nick_broadcast_shared_channel(self):
        """TRICKY: Vérifie que si Alice change de nom, Bob (sur le même canal) est prévenu"""
        alice = self.connect_and_register("Alice")
        bob = self.connect_and_register("Bob")
        
        # 1. Ils se rejoignent sur #general
        alice.sendall(b"JOIN #general\r\n")
        bob.sendall(b"JOIN #general\r\n")
        time.sleep(0.1)
        
        # On vide les buffers (messages de JOIN, listes des membres)
        alice.recv(4096)
        bob.recv(4096)
        
        # 2. Alice change de pseudo (ATTENTION : Max 9 caractères selon la RFC)
        alice.sendall(b"NICK Alix\r\n")
        time.sleep(0.1)
        
        # 3. Sécurité anti-freeze : on donne 2 secondes max à Bob pour recevoir la réponse
        bob.settimeout(2.0)
        try:
            bob_response = bob.recv(4096).decode("utf-8")
        except socket.timeout:
            self.fail("Bob n'a rien reçu ! Le broadcast a échoué (ou le changement de Nick a été refusé).")
        
        alice.close()
        bob.close()
        
        # Bob DOIT avoir reçu le message de changement de pseudo
        self.assertIn(":Alice", bob_response, "Bob n'a pas vu l'ancien pseudo d'Alice")
        self.assertIn("NICK :Alix", bob_response, "Bob n'a pas été notifié du changement (Broadcast défaillant)")
		
if __name__ == '__main__':
    unittest.main()