import socket
import time
import unittest
import random

SERVER = "127.0.0.1"
PORT = 6667
PASSWORD = "testpass"

class TestTopicCommand(unittest.TestCase):
    
    def connect_client(self, nick_base):
        short_base = nick_base[:4]
        nick = f"{short_base}{random.randint(1000, 9999)}"
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(3.0)
        s.connect((SERVER, PORT))
        s.send(f"PASS {PASSWORD}\r\nNICK {nick}\r\nUSER {nick} 0 * :Test User\r\n".encode("utf-8"))
        
        buffer = ""
        try:
            while "001" not in buffer:
                chunk = s.recv(4096).decode("utf-8")
                if not chunk: break
                buffer += chunk
                if "432" in buffer or "433" in buffer:
                    self.fail(f"Pseudo invalide ou déjà pris : {nick}")
        except socket.timeout:
            self.fail(f"Timeout lors de l'enregistrement de {nick}")
            
        return s, nick

    def join_channel(self, client, chan_name):
        client.send(f"JOIN {chan_name}\r\n".encode("utf-8"))
        buffer = ""
        try:
            while "366" not in buffer: # Attendre RPL_ENDOFNAMES
                chunk = client.recv(4096).decode("utf-8")
                if not chunk: break
                buffer += chunk
        except socket.timeout:
            self.fail(f"Timeout lors du JOIN de {chan_name}")

    def test_topic_missing_params(self):
        """Vérifie l'erreur 461 si la commande TOPIC est envoyée sans argument"""
        client, _ = self.connect_client("TopM")
        client.send(b"TOPIC\r\n")
        try:
            resp = client.recv(4096).decode("utf-8")
            self.assertIn("461", resp, "Doit renvoyer ERR_NEEDMOREPARAMS (461)")
        except socket.timeout:
            self.fail("Timeout sur test_topic_missing_params")
        finally:
            client.close()

    def test_topic_nosuchchannel(self):
        """Vérifie l'erreur 403 si le canal n'existe pas"""
        client, _ = self.connect_client("TopC")
        chan_name = f"#ghost{random.randint(1000, 9999)}"
        client.send(f"TOPIC {chan_name}\r\n".encode("utf-8"))
        try:
            resp = client.recv(4096).decode("utf-8")
            self.assertIn("403", resp, "Doit renvoyer ERR_NOSUCHCHANNEL (403)")
        except socket.timeout:
            self.fail("Timeout sur test_topic_nosuchchannel")
        finally:
            client.close()

    def test_topic_notonchannel(self):
        """Tricky: Vérifie l'erreur 442 si le client tente de lire le topic d'un canal qu'il n'a pas rejoint"""
        op_client, _ = self.connect_client("TopO")
        spy_client, _ = self.connect_client("Spy")
        chan_name = f"#secret{random.randint(1000, 9999)}"

        # Opérateur crée le canal
        self.join_channel(op_client, chan_name)
        
        # L'espion essaie de lire le topic sans avoir rejoint
        spy_client.send(f"TOPIC {chan_name}\r\n".encode("utf-8"))
        try:
            resp = spy_client.recv(4096).decode("utf-8")
            self.assertIn("442", resp, "Doit renvoyer ERR_NOTONCHANNEL (442)")
        except socket.timeout:
            self.fail("Timeout sur test_topic_notonchannel")
        finally:
            op_client.close()
            spy_client.close()

    def test_topic_set_and_clear(self):
        """Vérifie qu'on peut définir un topic (332) et l'effacer (331)"""
        client, _ = self.connect_client("TopS")
        chan_name = f"#talk{random.randint(1000, 9999)}"
        self.join_channel(client, chan_name)

        # 1. Définir le topic
        client.send(f"TOPIC {chan_name} :Ceci est un test\r\n".encode("utf-8"))
        time.sleep(0.1) # Laisser le temps au serveur de broadcast

        # 2. Lire le topic pour vérifier
        client.send(f"TOPIC {chan_name}\r\n".encode("utf-8"))
        try:
            resp = ""
            while "332" not in resp:
                chunk = client.recv(4096).decode("utf-8")
                if not chunk: break
                resp += chunk
            self.assertIn("Ceci est un test", resp, "Le topic n'a pas été sauvegardé (Manque 332 RPL_TOPIC)")
        except socket.timeout:
            self.fail("Timeout lors de la lecture du topic")

        # 3. Tricky: Effacer le topic
        client.send(f"TOPIC {chan_name} :\r\n".encode("utf-8"))
        time.sleep(0.1)
        
        # 4. Lire le topic pour vérifier l'effacement (doit renvoyer 331)
        client.send(f"TOPIC {chan_name}\r\n".encode("utf-8"))
        try:
            resp = ""
            while "331" not in resp:
                chunk = client.recv(4096).decode("utf-8")
                if not chunk: break
                resp += chunk
            self.assertIn("331", resp, "Le serveur doit renvoyer RPL_NOTOPIC (331) après effacement")
        except socket.timeout:
            self.fail("Timeout lors de la vérification de l'effacement")
        finally:
            client.close()

    def test_topic_protected_mode_t(self):
        """Tricky: Vérifie la restriction +t (482) pour les utilisateurs normaux"""
        op_client, _ = self.connect_client("ChanOp")
        normal_client, _ = self.connect_client("Pleb")
        chan_name = f"#protect{random.randint(1000, 9999)}"

        self.join_channel(op_client, chan_name)
        self.join_channel(normal_client, chan_name)

        # 1. L'opérateur active le mode +t (Topic restreint)
        op_client.send(f"MODE {chan_name} +t\r\n".encode("utf-8"))
        time.sleep(0.1)

        # 2. Le Pleb essaie de modifier le topic
        normal_client.send(f"TOPIC {chan_name} :Je hack le topic\r\n".encode("utf-8"))
        
        try:
            resp = ""
            while "482" not in resp and "TOPIC" not in resp:
                chunk = normal_client.recv(4096).decode("utf-8")
                if not chunk: break
                resp += chunk
            self.assertIn("482", resp, "Doit renvoyer ERR_CHANOPRIVSNEEDED (482)")
            self.assertNotIn("Je hack le topic", resp, "Le topic ne doit PAS être diffusé")
        except socket.timeout:
            self.fail("Timeout sur test_topic_protected_mode_t (Pleb)")

        # 3. L'opérateur modifie le topic (Doit réussir)
        op_client.send(f"TOPIC {chan_name} :Topic officiel\r\n".encode("utf-8"))
        try:
            resp = ""
            while "Topic officiel" not in resp:
                chunk = op_client.recv(4096).decode("utf-8")
                if not chunk: break
                resp += chunk
            self.assertIn("TOPIC", resp, "L'opérateur doit réussir à changer le topic")
        except socket.timeout:
            self.fail("Timeout sur test_topic_protected_mode_t (Op)")
        finally:
            op_client.close()
            normal_client.close()

if __name__ == '__main__':
    unittest.main()