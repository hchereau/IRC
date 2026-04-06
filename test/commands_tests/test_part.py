import socket
import time
import unittest

SERVER = "127.0.0.1"
PORT = 6667
PASSWORD = "password" # Adapte selon ton serveur

class TestPartCommand(unittest.TestCase):

    def connect_and_register(self, nickname):
        """Fonction utilitaire pour connecter et enregistrer un client."""
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((SERVER, PORT))
        sock.send(f"PASS {PASSWORD}\r\n".encode())
        sock.send(f"NICK {nickname}\r\n".encode())
        sock.send(f"USER {nickname} 0 * :Test User\r\n".encode())
        time.sleep(0.1)
        sock.recv(4096) # Vide le buffer
        return sock

    def test_basic_part(self):
        """Test Basique : Rejoindre puis Quitter"""
        client = self.connect_and_register("TP1")
        
        client.send(b"JOIN #test_part\r\n")
        time.sleep(0.1)
        client.recv(4096)
        
        client.send(b"PART #test_part\r\n")
        time.sleep(0.1)
        response = client.recv(4096).decode()
        
        self.assertIn("PART #test_part", response, "Le message de PART n'a pas été reçu.")
        client.close()

    def test_part_with_reason(self):
        """Test Tricky : Quitter avec un message (Trailing)"""
        client = self.connect_and_register("TP2")
        
        client.send(b"JOIN #test_reason\r\n")
        time.sleep(0.1)
        client.recv(4096)
        
        client.send(b"PART #test_reason :Je dois y aller, a plus!\r\n")
        time.sleep(0.1)
        response = client.recv(4096).decode()
        
        self.assertIn("PART #test_reason :Je dois y aller, a plus!", response, "Le trailing de PART a été mal parsé.")
        client.close()

    def test_part_nosuchchannel(self):
        """Test Tricky : Canal inexistant (403)"""
        client = self.connect_and_register("TP3")
        
        client.send(b"PART #ghostchannel\r\n")
        time.sleep(0.1)
        response = client.recv(4096).decode()
        
        self.assertIn("403", response, "ERR_NOSUCHCHANNEL n'a pas été renvoyé.")
        client.close()

    def test_part_notonchannel(self):
        """Test Tricky : Pas sur le canal (442)"""
        client1 = self.connect_and_register("TP4")
        client1.send(b"JOIN #mychan\r\n")
        time.sleep(0.1)
        client1.recv(4096)
        
        client2 = self.connect_and_register("TP5")
        client2.send(b"PART #mychan\r\n")
        time.sleep(0.1)
        response = client2.recv(4096).decode()
        
        self.assertIn("442", response, "ERR_NOTONCHANNEL n'a pas été renvoyé.")
        
        client1.close()
        client2.close()

if __name__ == '__main__':
    unittest.main()