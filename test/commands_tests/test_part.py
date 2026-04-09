import socket
import time
import unittest

SERVER = "127.0.0.1"
PORT = 6667
PASSWORD = "testpass"

class TestPartCommand(unittest.TestCase):
    
    def connect_and_register(self, nickname):
        """Fonction utilitaire pour connecter et enregistrer un client."""
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((SERVER, PORT))
        sock.sendall(f"PASS {PASSWORD}\r\n".encode())
        sock.sendall(f"NICK {nickname}\r\n".encode())
        sock.sendall(f"USER {nickname} 0 * :Test User\r\n".encode())
        time.sleep(0.1)
        sock.recv(4096)
        return sock

    def test_basic_part(self):
        """Test Basique : Rejoindre puis Quitter"""
        client = self.connect_and_register("TP1")
        
        client.sendall(b"JOIN #test_part\r\n")
        time.sleep(0.1)
        client.recv(4096)
        
        client.sendall(b"PART #test_part\r\n")
        time.sleep(0.1)
        response = client.recv(4096).decode()
        
        self.assertIn("PART #test_part", response, "Le message de PART n'a pas été reçu.")
        client.close()

    def test_part_with_reason(self):
        """Test Tricky : Quitter avec un message (Trailing)"""
        client = self.connect_and_register("TP2")
        
        client.sendall(b"JOIN #test_reason\r\n")
        time.sleep(0.1)
        client.recv(4096)
        
        client.sendall(b"PART #test_reason :Je dois y aller, a plus!\r\n")
        time.sleep(0.1)
        response = client.recv(4096).decode()
        
        self.assertIn("PART #test_reason :Je dois y aller, a plus!", response, "Le trailing de PART a été mal parsé.")
        client.close()

    def test_part_nosuchchannel(self):
        """Test Tricky : Canal inexistant (403)"""
        client = self.connect_and_register("TP3")
        
        client.sendall(b"PART #ghostchannel\r\n")
        time.sleep(0.1)
        response = client.recv(4096).decode()
        
        self.assertIn("403", response, "ERR_NOSUCHCHANNEL n'a pas été renvoyé.")
        client.close()

    def test_part_notonchannel(self):
        """Test Tricky : Pas sur le canal (442)"""
        client1 = self.connect_and_register("TP4")
        client1.sendall(b"JOIN #mychan\r\n")
        time.sleep(0.1)
        client1.recv(4096)
        
        client2 = self.connect_and_register("TP5")
        client2.sendall(b"PART #mychan\r\n")
        time.sleep(0.1)
        response = client2.recv(4096).decode()
        
        self.assertIn("442", response, "ERR_NOTONCHANNEL n'a pas été renvoyé.")
        
        client1.close()
        client2.close()
    
    def test_part_empty_channel_destruction(self):
        """Vérifie que lorsqu'un canal se vide suite à un PART, il est bien détruit par le serveur"""
        alice = self.connect_and_register("Alice")
        alice.sendall(b"JOIN #zombie\r\n")
        time.sleep(0.1)
        alice.recv(4096)
        
        alice.sendall(b"PART #zombie\r\n")
        time.sleep(0.1)
        alice.recv(4096)
        
        bob = self.connect_and_register("Bob")
        bob.sendall(b"JOIN #zombie\r\n")
        time.sleep(0.1)
        
        response = bob.recv(4096).decode("utf-8")
        
        alice.close()
        bob.close()
        
        self.assertIn("@Bob", response, "Le canal vide n'a pas été détruit ! Bob a rejoint un canal zombie au lieu d'en recréer un.")

if __name__ == '__main__':
    unittest.main()