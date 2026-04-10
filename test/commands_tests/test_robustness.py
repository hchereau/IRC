import socket
import time
import unittest

SERVER = "127.0.0.1"
PORT = 6667
PASSWORD = "testpass"

class TestRobustnessAndSecurity(unittest.TestCase):
    
    def connect_client(self, nick):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((SERVER, PORT))
        s.send(f"PASS {PASSWORD}\r\nNICK {nick}\r\nUSER {nick} 0 * :Test User\r\n".encode("utf-8"))
        time.sleep(0.1)
        s.recv(4096) # Vider le buffer
        return s

    def test_network_fragmentation(self):
        """Vérifie que le serveur gère bien une commande envoyée morceau par morceau (simulation de lag/nc)"""
        client = self.connect_client("FragClient")
        
        # On envoie la commande JOIN lettre par lettre avec un délai
        command_parts = [b"J", b"O", b"I", b"N", b" ", b"#", b"f", b"r", b"a", b"g", b"\r", b"\n"]
        
        for part in command_parts:
            client.send(part)
            time.sleep(0.05) # Délai artificiel pour forcer le serveur à utiliser le readBuffer
            
        time.sleep(0.1)
        resp = client.recv(4096).decode("utf-8")
        
        # Le serveur ne doit traiter la commande qu'une fois le \r\n reçu
        self.assertIn("JOIN :#frag", resp, "Le serveur n'a pas réussi à recomposer la commande fragmentée")
        client.close()

    def test_flood_protection(self):
        """Vérifie que le serveur déconnecte un client qui envoie un message trop long sans \\r\\n (MAX_READ_BUFF)"""
        client = self.connect_client("FloodClient")
        
        massive_string = b"A" * 8192
        client.send(massive_string)
        time.sleep(0.2)
        
        # On essaie d'envoyer une commande valide ensuite
        try:
            client.send(b"PING :test\r\n")
            time.sleep(0.1)
            resp = client.recv(4096)
            
            # Si resp est vide (b''), cela signifie que le serveur a fermé la connexion, ce qui est le comportement attendu.
            self.assertEqual(resp, b'', "Le serveur n'a pas déconnecté le client après un flood massif")
        except BrokenPipeError:
            # Si une exception BrokenPipeError est levée, la socket a bien été fermée par le serveur (Succès)
            pass 
        finally:
            client.close()

    def test_operator_privilege_kick(self):
        """Vérifie qu'un utilisateur normal ne peut pas kicker l'opérateur (ERR_CHANOPRIVSNEEDED - 482)"""
        op_client = self.connect_client("ChanOp")
        normal_client = self.connect_client("Pleb")

        # ChanOp rejoint en premier et devient opérateur (@)
        op_client.send(b"JOIN #dictature\r\n")
        time.sleep(0.1)
        op_client.recv(4096)

        # Pleb rejoint ensuite
        normal_client.send(b"JOIN #dictature\r\n")
        time.sleep(0.1)
        normal_client.recv(4096)
        op_client.recv(4096) # ChanOp voit Pleb arriver

        # Pleb essaie de kicker ChanOp (ce qui est illégal)
        normal_client.send(b"KICK #dictature ChanOp :Je prends le pouvoir\r\n")
        time.sleep(0.1)
        resp = normal_client.recv(4096).decode("utf-8")
        
        self.assertIn("482", resp, "Doit renvoyer ERR_CHANOPRIVSNEEDED (482) car Pleb n'est pas opérateur")

        op_client.close()
        normal_client.close()

if __name__ == '__main__':
    unittest.main()