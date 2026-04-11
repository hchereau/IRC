import socket
import time
import unittest
import random

SERVER = "127.0.0.1"
PORT = 6667
PASSWORD = "testpass"

class TestRobustnessAndSecurity(unittest.TestCase):
    
    def connect_client(self, nick_base):
        # On garantit un pseudo de 8 caractères max (RFC 2812 = 9 caractères max)
        short_base = nick_base[:4]
        nick = f"{short_base}{random.randint(1000, 9999)}"
        
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(3.0) # TIMEOUT GLOBAL
        s.connect((SERVER, PORT))
        s.send(f"PASS {PASSWORD}\r\nNICK {nick}\r\nUSER {nick} 0 * :Test User\r\n".encode("utf-8"))
        
        buffer = ""
        try:
            while "001" not in buffer and "433" not in buffer:
                chunk = s.recv(4096).decode("utf-8")
                if not chunk: break
                buffer += chunk
                
                # Si le serveur rejette le pseudo, on coupe court pour éviter le timeout
                if "432" in buffer:
                    self.fail(f"Pseudo invalide rejeté par le serveur (432) : {nick}")
                    
        except socket.timeout:
            self.fail(f"Timeout lors de l'enregistrement de {nick}")
            
        return s, nick

    def test_network_fragmentation(self):
        """Vérifie que le serveur gère bien une commande envoyée morceau par morceau"""
        client, _ = self.connect_client("Frag")
        chan_name = f"#frg{random.randint(1000, 9999)}"
        
        cmd = f"JOIN {chan_name}\r\n".encode('utf-8')
        command_parts = [bytes([b]) for b in cmd]
        
        for part in command_parts:
            client.send(part)
            time.sleep(0.05) 
            
        try:
            resp = ""
            expected = f"JOIN :{chan_name}"
            while expected not in resp:
                chunk = client.recv(4096).decode("utf-8")
                if not chunk: break
                resp += chunk
            self.assertIn(expected, resp, "Le serveur n'a pas réussi à recomposer la commande fragmentée")
        except socket.timeout:
            self.fail("Timeout : Le serveur n'a jamais répondu à la commande fragmentée")
        finally:
            client.close()

    def test_flood_protection(self):
        """Vérifie que le serveur déconnecte un client qui flood sans \\r\\n"""
        client, _ = self.connect_client("Flod")
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
        op_client, op_nick = self.connect_client("ChOp")
        normal_client, normal_nick = self.connect_client("Pleb")
        chan_name = f"#dic{random.randint(1000, 9999)}"

        # 1. ChOp rejoint
        op_client.send(f"JOIN {chan_name}\r\n".encode("utf-8"))
        buffer = ""
        try:
            while "366" not in buffer:
                chunk = op_client.recv(4096).decode("utf-8")
                if not chunk: break
                buffer += chunk
        except socket.timeout:
            self.fail("Timeout : ChOp n'a pas reçu la confirmation (366) de son JOIN")

        # 2. Pleb rejoint
        normal_client.send(f"JOIN {chan_name}\r\n".encode("utf-8"))
        buffer = ""
        try:
            while "366" not in buffer:
                chunk = normal_client.recv(4096).decode("utf-8")
                if not chunk: break
                buffer += chunk
        except socket.timeout:
            self.fail("Timeout : Pleb n'a pas reçu la confirmation (366) de son JOIN")

        # 3. Pleb tente le kick
        kick_cmd = f"KICK {chan_name} {op_nick} :Je prends le pouvoir\r\n"
        normal_client.send(kick_cmd.encode("utf-8"))
        
        try:
            resp = ""
            while "482" not in resp and "441" not in resp and "401" not in resp:
                chunk = normal_client.recv(4096).decode("utf-8")
                if not chunk: break
                resp += chunk
                
            self.assertIn("482", resp, f"Doit renvoyer ERR_CHANOPRIVSNEEDED (482). Reçu: {resp}")
        except socket.timeout:
            self.fail("Timeout : Pas de réponse lors du KICK")
        finally:
            op_client.close()
            normal_client.close()

if __name__ == '__main__':
    unittest.main()