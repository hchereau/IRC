import socket
import time
import unittest

# --- CONFIGURATION ---
HOST = '127.0.0.1'
PORT = 6667
PASS = 'testpass'

class TestModeCommand(unittest.TestCase):

    def setUp(self):
        """Initialise la connexion d'Alice (Opératrice) et Bob (Membre normal) avant chaque test."""
        # Connexion d'Alice
        self.alice = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.alice.connect((HOST, PORT))
        self._send_msg(self.alice, f"PASS {PASS}")
        self._send_msg(self.alice, "NICK Alice")
        self._send_msg(self.alice, "USER alice 0 * :Alice")

        # Connexion de Bob
        self.bob = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.bob.connect((HOST, PORT))
        self._send_msg(self.bob, f"PASS {PASS}")
        self._send_msg(self.bob, "NICK Bob")
        self._send_msg(self.bob, "USER bob 0 * :Bob")

        time.sleep(0.2)
        self._recv_msg(self.alice) # Vider le buffer des messages de bienvenue
        self._recv_msg(self.bob)

        # Création du canal par Alice (devient Opératrice)
        self._send_msg(self.alice, "JOIN #modetest")
        self._recv_msg(self.alice)
        
        # Bob rejoint le canal
        self._send_msg(self.bob, "JOIN #modetest")
        self._recv_msg(self.bob)
        self._recv_msg(self.alice) # Vider la notif JOIN pour Alice

    def tearDown(self):
        """Ferme les sockets après chaque test."""
        self._send_msg(self.alice, "QUIT :Bye")
        self._send_msg(self.bob, "QUIT :Bye")
        self.alice.close()
        self.bob.close()

    def _send_msg(self, sock, msg):
        sock.send((msg + "\r\n").encode('utf-8'))
        time.sleep(0.1) # Léger délai pour éviter la fusion TCP (DTO)

    def _recv_msg(self, sock):
        sock.settimeout(0.5)
        try:
            return sock.recv(4096).decode('utf-8')
        except socket.timeout:
            return ""

    # --- DÉBUT DES TESTS ---

    def test_mode_check_modes(self):
        """Vérifie la consultation des modes actuels d'un canal (RPL_CHANNELMODEIS 324)"""
        self._send_msg(self.alice, "MODE #modetest")
        resp = self._recv_msg(self.alice)
        self.assertIn("324", resp, "Le serveur doit renvoyer le code 324 pour la consultation des modes.")

    def test_mode_err_chanoprivsneeded(self):
        """Vérifie l'erreur 482 (ERR_CHANOPRIVSNEEDED) : Bob tente de modifier un mode sans être opérateur"""
        self._send_msg(self.bob, "MODE #modetest +i")
        resp = self._recv_msg(self.bob)
        self.assertIn("482", resp, "Bob ne devrait pas pouvoir modifier le mode du canal.")

    def test_mode_add_invite(self):
        """Vérifie l'ajout du mode Invite (+i) par une opératrice"""
        self._send_msg(self.alice, "MODE #modetest +i")
        resp = self._recv_msg(self.alice)
        self.assertIn("MODE #modetest +i", resp)

    def test_mode_remove_invite(self):
            """Vérifie le retrait du mode Invite (-i)"""
            # 1. On active d'abord le mode (car le channel est neuf à chaque test)
            self._send_msg(self.alice, "MODE #modetest +i")
            self._recv_msg(self.alice) # On vide le buffer de la confirmation +i

            # 2. Maintenant on teste le retrait effectif
            self._send_msg(self.alice, "MODE #modetest -i")
            resp = self._recv_msg(self.alice)
            self.assertIn("MODE #modetest -i", resp, "Le serveur doit confirmer le retrait du mode -i")
            
    def test_mode_add_key(self):
        """Vérifie l'ajout d'une clé/mot de passe au canal (+k)"""
        self._send_msg(self.alice, "MODE #modetest +k secretpass")
        resp = self._recv_msg(self.alice)
        self.assertIn("+k secretpass", resp)

    def test_mode_add_operator(self):
        """Vérifie l'élévation des privilèges d'un membre vers opérateur (+o)"""
        self._send_msg(self.alice, "MODE #modetest +o Bob")
        resp = self._recv_msg(self.alice)
        self.assertIn("+o Bob", resp)

    def test_mode_add_limit(self):
        """Vérifie la configuration d'une limite d'utilisateurs (+l)"""
        self._send_msg(self.alice, "MODE #modetest +l 10")
        resp = self._recv_msg(self.alice)
        self.assertIn("+l 10", resp)

    def test_mode_unknown_mode(self):
        """Vérifie le rejet d'un mode inconnu avec l'erreur 472 (ERR_UNKNOWNMODE)"""
        self._send_msg(self.alice, "MODE #modetest +z")
        resp = self._recv_msg(self.alice)
        self.assertIn("472", resp, "Le serveur doit renvoyer 472 pour un mode invalide.")

    def test_mode_multiple_modes(self):
        """Vérifie l'application d'une chaîne de modes multiples (ex: +t-i)"""
        self._send_msg(self.alice, "MODE #modetest +t-i")
        resp = self._recv_msg(self.alice)
        self.assertTrue("+t" in resp or "-i" in resp, "Les modes multiples doivent être traités.")

if __name__ == '__main__':
    unittest.main()