import socket
import time
import unittest
import random

SERVER = "127.0.0.1"
PORT = 6667
PASSWORD = "testpass"

class TestRobustnessAndSecurity(unittest.TestCase):
    
    def connect_client(self, nick_base):
        # On génère un pseudo unique
        nick = f"{nick_base}_{random.randint(1000, 9999)}"
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(3.0) # TIMEOUT GLOBAL
        s.connect((SERVER, PORT))
        s.send(f"PASS {PASSWORD}\r\nNICK {nick}\r\nUSER {nick} 0 * :Test User\r\n".encode("utf-8"))
        
        buffer = ""
        try:
            while "001" not in buffer and "433" not in buffer:
                buffer += s.recv(4096).decode("utf-8")
        except socket.timeout:
            pass
            
        # On retourne le socket ET le pseudo exact généré
        return s, nick

    def test_network_fragmentation(self):
        """Vérifie que le serveur gère bien une commande envoyée morceau par morceau"""
        client, _ = self.connect_client("Frag")
        
        command_parts = [b"J", b"O", b"I", b"N", b" ", b"#", b"f", b"r", b"a", b"g", b"\r", b"\n"]
        
        for part in command_parts:
            client.send(part)
            time.sleep(0.1) 
            
        try:
            resp = ""
            while "JOIN :#frag" not in resp:
                chunk = client.recv(4096).decode("utf-8")
                if not chunk:
                    break
                resp += chunk
            self.assertIn("JOIN :#frag", resp, "Le serveur n'a pas réussi à recomposer la commande")
        except socket.timeout:
            self.fail("Timeout : Le serveur n'a jamais répondu à la commande fragmentée")
        finally:
            client.close()

    def test_flood_protection(self):
        """Vérifie que le serveur déconnecte un client qui flood sans \\r\\n"""
        client, _ = self.connect_client("Flood")
        massive_string = b"A" * 4500
        client.send(massive_string)
        time.sleep(0.5)
        
        try:
            client.send(b"PING :test\r\n")
            resp = client.recv(4096)
            self.assertEqual(resp, b'')
        except (BrokenPipeError, ConnectionResetError, socket.timeout):
            pass 
        finally:
            client.close()

    def test_operator_privilege_kick(self):
        """Vérifie qu'un utilisateur normal ne peut pas kicker l'opérateur"""
        op_client, op_nick = self.connect_client("ChanOp")
        normal_client, normal_nick = self.connect_client("Pleb")

        # 1. ChanOp rejoint. On attend la confirmation (366 End of NAMES) pour être SÛR qu'il a créé le channel
        op_client.send(b"JOIN #dictature\r\n")
        buffer = ""
        try:
            while "366" not in buffer:
                buffer += op_client.recv(4096).decode("utf-8")
        except socket.timeout:
            pass

        # 2. Pleb rejoint. On vide son buffer pour éviter qu'il lise son propre JOIN plus tard
        normal_client.send(b"JOIN #dictature\r\n")
        buffer = ""
        try:
            while "366" not in buffer:
                buffer += normal_client.recv(4096).decode("utf-8")
        except socket.timeout:
            pass

        # 3. Pleb tente de kicker ChanOp avec son pseudo exact
        kick_cmd = f"KICK #dictature {op_nick} :Je prends le pouvoir\r\n"
        normal_client.send(kick_cmd.encode("utf-8"))
        
        try:
            resp = ""
            # On cherche l'erreur 482 (ou on s'arrête si le serveur nous donne un autre code inattendu)
            while "482" not in resp and "441" not in resp and "401" not in resp:
                chunk = normal_client.recv(4096).decode("utf-8")
                if not chunk:
                    break
                resp += chunk
                
            self.assertIn("482", resp, f"Doit renvoyer ERR_CHANOPRIVSNEEDED (482). Reçu: {resp}")
        except socket.timeout:
            self.fail("Timeout : Pas de réponse lors du KICK")
        finally:
            op_client.close()
            normal_client.close()

if __name__ == '__main__':
    unittest.main()