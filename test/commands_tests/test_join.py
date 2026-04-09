import socket
import time
import unittest

SERVER = "127.0.0.1"
PORT = 6667
PASSWORD = "testpass"

class TestJoinCommand(unittest.TestCase):
    
    def connect_client(self, nick):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((SERVER, PORT))
        s.send(f"PASS {PASSWORD}\r\nNICK {nick}\r\nUSER {nick} 0 * :Test User\r\n".encode("utf-8"))
        time.sleep(0.1)
        # Vider le buffer des messages de bienvenue (001, etc.)
        s.recv(4096) 
        return s

    def test_join_logic(self):
        # 1. Connexion de Client A
        clientA = self.connect_client("ClientA")
        
        # Client A rejoint #test
        clientA.send(b"JOIN #test\r\n")
        time.sleep(0.1)
        respA = clientA.recv(4096).decode("utf-8")
        
        self.assertIn("JOIN :#test", respA, "ClientA devrait recevoir le broadcast de son propre JOIN")
        self.assertIn("353 ClientA = #test :@ClientA", respA, "ClientA devrait être @ opérateur")
        self.assertIn("366 ClientA #test :End of /NAMES list", respA, "ClientA devrait recevoir ENDOFNAMES")

        # 2. Connexion de Client B
        clientB = self.connect_client("ClientB")
        
        # Client B rejoint le même canal
        clientB.send(b"JOIN #test\r\n")
        time.sleep(0.1)
        respB = clientB.recv(4096).decode("utf-8")
        
        self.assertIn("JOIN :#test", respB, "ClientB devrait recevoir le broadcast de son propre JOIN")
        
        # Vérification du NAMREPLY pour ClientB (l'ordre des pseudos peut varier)
        names_ok = "353 ClientB = #test :@ClientA ClientB" in respB or "353 ClientB = #test :ClientB @ClientA" in respB
        self.assertTrue(names_ok, f"Le NAMREPLY est incorrect. Reçu : {respB}")

        # 3. Vérification du Broadcast chez Client A
        respA_broadcast = clientA.recv(4096).decode("utf-8")
        
        broadcast_ok = "ClientB!ClientB" in respA_broadcast and "JOIN :#test" in respA_broadcast
        self.assertTrue(broadcast_ok, "Client A n'a pas reçu le message de JOIN de Client B")

        # Nettoyage
        clientA.close()
        clientB.close()
    def test_join_missing_params(self):
        """Vérifie l'erreur 461 si aucun argument n'est fourni"""
        client = self.connect_client("ClientMiss")
        client.send(b"JOIN\r\n")
        time.sleep(0.1)
        resp = client.recv(4096).decode("utf-8")
        
        self.assertIn("461", resp, "Doit renvoyer ERR_NEEDMOREPARAMS (461)")
        client.close()

    def test_join_invalid_name(self):
        """Vérifie l'erreur 403 si le canal ne commence pas par # ou &"""
        client = self.connect_client("ClitInv")
        client.send(b"JOIN badchannel\r\n")
        time.sleep(0.1)
        resp = client.recv(4096).decode("utf-8")
        
        self.assertIn("403", resp, "Doit renvoyer ERR_NOSUCHCHANNEL (403)")
        client.close()

    def test_join_multiple_channels(self):
        """Vérifie qu'un client peut rejoindre plusieurs canaux séparés par une virgule"""
        nick = "CliMulti" # On définit le pseudo une seule fois ici !
        client = self.connect_client(nick)
        
        client.send(b"JOIN #chan1,#chan2\r\n")
        time.sleep(0.1)
        resp = client.recv(4096).decode("utf-8")
        
        # Le client doit recevoir un JOIN et un NAMREPLY pour chaque canal
        self.assertIn("JOIN :#chan1", resp, "Doit avoir rejoint #chan1")
        self.assertIn(f"353 {nick} = #chan1", resp, "Doit recevoir le NAMREPLY de #chan1")
        
        self.assertIn("JOIN :#chan2", resp, "Doit avoir rejoint #chan2")
        self.assertIn(f"353 {nick} = #chan2", resp, "Doit recevoir le NAMREPLY de #chan2")
        client.close()

    def test_join_asymmetric_keys(self):
        """Vérifie que la séparation des clés fonctionne même si les listes sont asymétriques"""
        client = self.connect_client("ClitKeys")
        # 2 canaux, 1 seule clé. Le parseur ne doit pas faire de segfault
        client.send(b"JOIN #kchan1,#kchan2 mykey\r\n")
        time.sleep(0.1)
        resp = client.recv(4096).decode("utf-8")
        
        self.assertIn("JOIN :#kchan1", resp)
        self.assertIn("JOIN :#kchan2", resp)
        client.close()

    def test_join_err_badchannelkey(self):
        """Vérifie l'erreur 475 si le mot de passe du canal (+k) est manquant ou incorrect"""
        alice = self.connect_client("Alice")
        bob = self.connect_client("Bob")

        # Alice crée le canal et met un mot de passe
        alice.send(b"JOIN #vault\r\n")
        time.sleep(0.1)
        alice.recv(4096)
        
        alice.send(b"MODE #vault +k secret\r\n")
        time.sleep(0.1)
        alice.recv(4096)

        # Bob tente de rejoindre sans mot de passe
        bob.send(b"JOIN #vault\r\n")
        time.sleep(0.1)
        resp1 = bob.recv(4096).decode("utf-8")
        self.assertIn("475", resp1, "Doit renvoyer ERR_BADCHANNELKEY (sans clé)")

        # Bob tente avec le mauvais mot de passe
        bob.send(b"JOIN #vault wrongpass\r\n")
        time.sleep(0.1)
        resp2 = bob.recv(4096).decode("utf-8")
        self.assertIn("475", resp2, "Doit renvoyer ERR_BADCHANNELKEY (mauvaise clé)")

        # Bob tente avec le bon mot de passe
        bob.send(b"JOIN #vault secret\r\n")
        time.sleep(0.1)
        resp3 = bob.recv(4096).decode("utf-8")
        self.assertIn("JOIN", resp3, "Bob doit pouvoir rejoindre avec la bonne clé")

        alice.close()
        bob.close()

    def test_join_err_channelisfull(self):
        """Vérifie l'erreur 471 si la limite d'utilisateurs (+l) est atteinte"""
        alice = self.connect_client("Alice")
        bob = self.connect_client("Bob")

        # Alice crée le canal et met une limite de 1 personne
        alice.send(b"JOIN #vip\r\n")
        time.sleep(0.1)
        alice.recv(4096)
        
        alice.send(b"MODE #vip +l 1\r\n")
        time.sleep(0.1)
        alice.recv(4096)

        # Bob tente de rejoindre alors que la limite de 1 (Alice) est atteinte
        bob.send(b"JOIN #vip\r\n")
        time.sleep(0.1)
        resp = bob.recv(4096).decode("utf-8")
        self.assertIn("471", resp, "Doit renvoyer ERR_CHANNELISFULL")

        alice.close()
        bob.close()

    def test_join_err_inviteonlychan(self):
        """Vérifie l'erreur 473 si le canal est sur invitation (+i) et le succès si invité"""
        alice = self.connect_client("Alice")
        bob = self.connect_client("Bob")

        # Alice crée le canal et le met en invite-only
        alice.send(b"JOIN #secret\r\n")
        time.sleep(0.1)
        alice.recv(4096)
        
        alice.send(b"MODE #secret +i\r\n")
        time.sleep(0.1)
        alice.recv(4096)

        # 1. Bob tente de rejoindre sans invitation
        bob.send(b"JOIN #secret\r\n")
        time.sleep(0.1)
        resp = bob.recv(4096).decode("utf-8")
        self.assertIn("473", resp, "Doit renvoyer ERR_INVITEONLYCHAN")

        # 2. Alice invite Bob
        alice.send(b"INVITE Bob #secret\r\n")
        time.sleep(0.1)
        alice.recv(4096)
        bob.recv(4096) # Bob reçoit la notif d'invitation

        # 3. Bob tente de rejoindre à nouveau (Doit réussir car invité)
        bob.send(b"JOIN #secret\r\n")
        time.sleep(0.1)
        resp_success = bob.recv(4096).decode("utf-8")
        self.assertIn("JOIN", resp_success, "Bob doit pouvoir rejoindre après invitation")

        # 4. TRICKY: L'invitation est consommée. Si Bob part et revient, il doit être bloqué.
        bob.send(b"PART #secret\r\n")
        time.sleep(0.1)
        bob.recv(4096)
        alice.recv(4096) # Alice voit Bob partir
        
        bob.send(b"JOIN #secret\r\n")
        time.sleep(0.1)
        resp_rejoin = bob.recv(4096).decode("utf-8")
        self.assertIn("473", resp_rejoin, "L'invitation doit être consommée après usage")

        alice.close()
        bob.close()
    
    def test_join_not_registered(self):
        """Vérifie l'erreur 451 ERR_NOTREGISTERED si le client n'est pas identifié"""
        # 1. Nouvelle connexion vierge (sans utiliser connect_client pour éviter le PASS/NICK/USER)
        ghost_client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        ghost_client.connect((SERVER, PORT))
        
        # 2. On tente un JOIN direct sans PASS, NICK ni USER
        ghost_client.sendall(b"JOIN #interdit\r\n")
        time.sleep(0.1)
        
        # 3. On lit la réponse et on ferme
        response = ghost_client.recv(4096).decode("utf-8")
        ghost_client.close()
        
        # 4. Vérification
        self.assertIn("451", response, "Le serveur doit renvoyer l'erreur 451 ERR_NOTREGISTERED")