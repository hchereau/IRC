import socket
import time
import unittest

SERVER = "127.0.0.1"
PORT = 6667
PASSWORD = "testpass"

class TestInviteCommand(unittest.TestCase):
    
    def connect_client(self, nick):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((SERVER, PORT))
        s.send(f"PASS {PASSWORD}\r\nNICK {nick}\r\nUSER {nick} 0 * :Test User\r\n".encode("utf-8"))
        time.sleep(0.1)
        # Vider le buffer de connexion
        s.recv(4096) 
        return s

    def test_invite_normal_behavior(self):
        """Cas normal: ClientA invite ClientB dans un canal qu'il a rejoint."""
        clientA = self.connect_client("InvA")
        clientB = self.connect_client("InvB")
        
        # ClientA rejoint le canal
        clientA.send(b"JOIN #inv_chan\r\n")
        time.sleep(0.1)
        clientA.recv(4096) # vide le buffer du JOIN
        
        # ClientA invite ClientB
        clientA.send(b"INVITE InvB #inv_chan\r\n")
        time.sleep(0.1)
        
        # Vérification côté Expéditeur (ClientA doit recevoir 341 RPL_INVITING)
        respA = clientA.recv(4096).decode("utf-8")
        self.assertIn("341", respA, "ClientA doit recevoir le code RPL_INVITING (341)")
        self.assertIn("InvB #inv_chan", respA, "Le format RPL_INVITING doit contenir le pseudo et le canal")
        
        # Vérification côté Destinataire (ClientB doit recevoir le message INVITE complet)
        respB = clientB.recv(4096).decode("utf-8")
        self.assertIn("INVITE InvB :#inv_chan", respB, "ClientB doit recevoir la notification d'invitation")
        self.assertIn(":InvA!", respB, "Le message d'invitation doit provenir de l'expéditeur (prefix)")

        clientA.close()
        clientB.close()

    def test_invite_missing_params(self):
        """Tricky: Vérifie l'erreur 461 si paramètres manquants."""
        client = self.connect_client("InvMiss")
        
        # Un seul paramètre
        client.send(b"INVITE OnlyOneParam\r\n")
        time.sleep(0.1)
        resp = client.recv(4096).decode("utf-8")
        self.assertIn("461", resp, "Doit renvoyer ERR_NEEDMOREPARAMS (461)")
        
        client.close()

    def test_invite_no_such_nick(self):
        """Tricky: Vérifie l'erreur 401 si le client ciblé n'existe pas."""
        client = self.connect_client("InvNoNick")
        
        client.send(b"JOIN #alone_chan\r\n")
        time.sleep(0.1)
        client.recv(4096)
        
        client.send(b"INVITE GhostUser #alone_chan\r\n")
        time.sleep(0.1)
        resp = client.recv(4096).decode("utf-8")
        
        self.assertIn("401", resp, "Doit renvoyer ERR_NOSUCHNICK (401)")
        client.close()

    def test_invite_not_on_channel(self):
        """Tricky: L'expéditeur tente d'inviter sur un canal dont il ne fait pas partie."""
        clientA = self.connect_client("InvOutA")
        clientB = self.connect_client("InvOutB")
        
        # --- AJOUT: ClientB crée et rejoint le canal pour qu'il existe côté serveur ---
        clientB.send(b"JOIN #some_chan\r\n")
        time.sleep(0.1)
        clientB.recv(4096)
        
        # ClientA n'a rejoint aucun canal mais tente d'inviter sur un canal EXISTANT
        clientA.send(b"INVITE InvOutB #some_chan\r\n")
        time.sleep(0.1)
        resp = clientA.recv(4096).decode("utf-8")
        
        self.assertIn("442", resp, "Doit renvoyer ERR_NOTONCHANNEL (442)")
        
        clientA.close()
        clientB.close()

    def test_invite_user_already_on_channel(self):
        """Tricky: L'utilisateur invité est déjà dans le canal."""
        clientA = self.connect_client("InvInA")
        clientB = self.connect_client("InvInB")
        
        # Les deux rejoignent le canal
        clientA.send(b"JOIN #shared_chan\r\n")
        clientB.send(b"JOIN #shared_chan\r\n")
        time.sleep(0.1)
        clientA.recv(4096)
        clientB.recv(4096)
        
        # A invite B (qui est déjà dedans)
        clientA.send(b"INVITE InvInB #shared_chan\r\n")
        time.sleep(0.1)
        resp = clientA.recv(4096).decode("utf-8")
        
        self.assertIn("443", resp, "Doit renvoyer ERR_USERONCHANNEL (443)")
        
        clientA.close()
        clientB.close()