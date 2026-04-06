import socket
import time
import unittest

SERVER = "127.0.0.1"
PORT = 6667
PASSWORD = "testpass"

class TestKickCommand(unittest.TestCase):
    
    def connect_client(self, nick):
        """Helper pour connecter et enregistrer un client rapidement"""
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(2.0)
        s.connect((SERVER, PORT))
        s.send(f"PASS {PASSWORD}\r\nNICK {nick}\r\nUSER {nick} 0 * :Test User\r\n".encode("utf-8"))
        time.sleep(0.1)
        s.recv(4096) # Vider le buffer (001, 002, etc.)
        return s

    # =========================================================================
    # TESTS NORMAUX (HAPPY PATH)
    # =========================================================================

    def test_kick_success_default_reason(self):
        """Un opérateur kick un utilisateur avec la raison par défaut"""
        op = self.connect_client("OpKicker") # 8 chars
        target = self.connect_client("Target1") # 7 chars
        
        op.send(b"JOIN #kickchan\r\n")
        time.sleep(0.1)
        op.recv(4096)
        
        target.send(b"JOIN #kickchan\r\n")
        time.sleep(0.1)
        target.recv(4096)
        op.recv(4096)
        
        op.send(b"KICK #kickchan Target1\r\n")
        time.sleep(0.1)
        
        resp_op = op.recv(4096).decode("utf-8")
        resp_target = target.recv(4096).decode("utf-8")
        
        kick_msg = "KICK #kickchan Target1 :"
        self.assertIn(kick_msg, resp_op)
        self.assertIn(kick_msg, resp_target)
        
        op.close()
        target.close()

    def test_kick_success_custom_reason(self):
        """Un opérateur kick avec une raison personnalisée (trailing parameter)"""
        op = self.connect_client("OpReason") # 8 chars
        target = self.connect_client("Target2") # 7 chars
        
        op.send(b"JOIN #reasonchan\r\n")
        time.sleep(0.1)
        op.recv(4096)
        
        target.send(b"JOIN #reasonchan\r\n")
        time.sleep(0.1)
        target.recv(4096)
        op.recv(4096)
        
        op.send(b"KICK #reasonchan Target2 :Tu parles trop\r\n")
        time.sleep(0.1)
        
        resp_op = op.recv(4096).decode("utf-8")
        resp_target = target.recv(4096).decode("utf-8")
        
        self.assertIn("KICK #reasonchan Target2 :Tu parles trop", resp_target)
        
        op.close()
        target.close()

    # =========================================================================
    # TESTS TRICKY & GESTION DES ERREURS
    # =========================================================================

    def test_kick_missing_params(self):
        """Vérifie l'erreur 461 (ERR_NEEDMOREPARAMS) : manque le nom de la cible"""
        client = self.connect_client("ErrMiss") # 7 chars
        client.send(b"KICK #chan_fantome\r\n")
        time.sleep(0.1)
        resp = client.recv(4096).decode("utf-8")
        
        self.assertIn("461", resp, "Doit renvoyer ERR_NEEDMOREPARAMS (461)")
        client.close()

    def test_kick_nosuchchannel(self):
        """Vérifie l'erreur 403 (ERR_NOSUCHCHANNEL) : le canal n'existe pas"""
        client = self.connect_client("ErrNoChan") # 9 chars
        client.send(b"KICK #nexistepas Fantome\r\n")
        time.sleep(0.1)
        resp = client.recv(4096).decode("utf-8")
        
        self.assertIn("403", resp, "Doit renvoyer ERR_NOSUCHCHANNEL (403)")
        client.close()

    def test_kick_notonchannel(self):
        """Vérifie l'erreur 442 (ERR_NOTONCHANNEL) : l'auteur du KICK n'est pas dans le canal"""
        chan_creator = self.connect_client("ChanOwner") # 9 chars
        outsider = self.connect_client("Outsider") # 8 chars
        
        chan_creator.send(b"JOIN #privchan\r\n")
        time.sleep(0.1)
        chan_creator.recv(4096)
        
        outsider.send(b"KICK #privchan ChanOwner\r\n")
        time.sleep(0.1)
        resp = outsider.recv(4096).decode("utf-8")
        
        self.assertIn("442", resp, "Doit renvoyer ERR_NOTONCHANNEL (442)")
        
        chan_creator.close()
        outsider.close()

    def test_kick_chanoprivsneeded(self):
        """Vérifie l'erreur 482 (ERR_CHANOPRIVSNEEDED) : un utilisateur normal tente de kicker"""
        op = self.connect_client("TrueOp") # 6 chars
        normal_user = self.connect_client("Normal") # 6 chars
        
        op.send(b"JOIN #opchan\r\n") 
        time.sleep(0.1)
        op.recv(4096)
        
        normal_user.send(b"JOIN #opchan\r\n") 
        time.sleep(0.1)
        normal_user.recv(4096)
        op.recv(4096)
        
        normal_user.send(b"KICK #opchan TrueOp :Je veux etre chef\r\n")
        time.sleep(0.1)
        resp = normal_user.recv(4096).decode("utf-8")
        
        self.assertIn("482", resp, "Doit renvoyer ERR_CHANOPRIVSNEEDED (482)")
        
        op.close()
        normal_user.close()

    def test_kick_usernotinchannel(self):
        """Vérifie l'erreur 441 (ERR_USERNOTINCHANNEL) : la cible n'est pas dans le canal"""
        op = self.connect_client("OpSeul") # 6 chars
        ninja = self.connect_client("NinjaUser") # 9 chars
        
        op.send(b"JOIN #solochan\r\n")
        time.sleep(0.1)
        op.recv(4096)
        
        op.send(b"KICK #solochan NinjaUser\r\n")
        time.sleep(0.1)
        resp = op.recv(4096).decode("utf-8")
        
        self.assertIn("441", resp, "Doit renvoyer ERR_USERNOTINCHANNEL (441)")
        
        op.close()
        ninja.close()