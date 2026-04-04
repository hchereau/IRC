import socket
import time
import unittest

SERVER = "127.0.0.1"
PORT = 6667
PASSWORD = "testpass"

class TestHandshake(unittest.TestCase):
    
    def test_successful_registration(self):
        """Vérifie qu'un client reçoit bien le 001 RPL_WELCOME"""
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((SERVER, PORT))
        
        sock.sendall(f"PASS {PASSWORD}\r\n".encode())
        sock.sendall(b"NICK testuser\r\n")
        sock.sendall(b"USER testuser 0 * :Real Name\r\n")
        
        time.sleep(0.1)
        response = sock.recv(4096).decode()
        sock.close()
        
        # assertIn vérifie que "001" est dans response
        self.assertIn("001", response, f"Le serveur n'a pas renvoyé 001. Réponse: {response}")

    def test_missing_parameters(self):
        """Vérifie que la commande USER échoue sans le Real Name"""
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((SERVER, PORT))
        
        sock.sendall(f"PASS {PASSWORD}\r\n".encode())
        sock.sendall(b"NICK testuser2\r\n")
        sock.sendall(b"USER testuser2 0 *\r\n") # Real name manquant
        
        time.sleep(0.1)
        response = sock.recv(4096).decode()
        sock.close()
        
        self.assertIn("461", response, f"Le serveur n'a pas renvoyé 461. Réponse: {response}")

    def test_wrong_password(self):
        """Vérifie que la connexion est refusée avec un mauvais mot de passe (ERR_PASSWDMISMATCH)"""
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((SERVER, PORT))
        
        sock.sendall(b"PASS wrong_password\r\n")
        
        time.sleep(0.1)
        response = sock.recv(4096).decode()
        sock.close()
        
        self.assertIn("464", response, f"Le serveur n'a pas renvoyé l'erreur 464. Réponse: {response}")

    def test_nick_already_in_use(self):
        """Vérifie qu'on ne peut pas prendre un pseudo déjà utilisé (ERR_NICKNAMEINUSE)"""
        # Client 1 s'enregistre
        sock1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock1.connect((SERVER, PORT))
        sock1.sendall(f"PASS {PASSWORD}\r\n".encode())
        sock1.sendall(b"NICK double_n\r\n")
        sock1.sendall(b"USER u1 0 * :Real\r\n")
        time.sleep(0.1)
        
        # Client 2 essaie de prendre le même pseudo
        sock2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock2.connect((SERVER, PORT))
        sock2.sendall(f"PASS {PASSWORD}\r\n".encode())
        sock2.sendall(b"NICK double_n\r\n")
        
        time.sleep(0.1)
        response = sock2.recv(4096).decode()
        
        sock1.close()
        sock2.close()
        
        self.assertIn("433", response, f"Le serveur n'a pas renvoyé l'erreur 433. Réponse: {response}")

    def test_invalid_nickname(self):
        """Vérifie le rejet d'un pseudo avec des caractères interdits (ERR_ERRONEUSNICKNAME)"""
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((SERVER, PORT))
        
        sock.sendall(f"PASS {PASSWORD}\r\n".encode())
        sock.sendall(b"NICK #invalN!c\r\n")
        
        time.sleep(0.1)
        response = sock.recv(4096).decode()
        sock.close()
        
        self.assertIn("432", response, f"Le serveur n'a pas renvoyé l'erreur 432. Réponse: {response}")

    def test_pass_already_registered(self):
        """Vérifie qu'un client enregistré ne peut pas renvoyer PASS (ERR_ALREADYREGISTRED)"""
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((SERVER, PORT))
        
        sock.sendall(f"PASS {PASSWORD}\r\nNICK testuser3\r\nUSER testuser3 0 * :Real Name\r\n".encode())
        time.sleep(0.1)
        sock.recv(4096) # Vide le buffer de réception (le 001)
        
        sock.sendall(b"PASS newpass\r\n")
        time.sleep(0.1)
        response = sock.recv(4096).decode()
        sock.close()
        
        self.assertIn("462", response, f"Le serveur n'a pas renvoyé l'erreur 462 pour PASS. Réponse: {response}")

    def test_user_already_registered(self):
        """Vérifie qu'un client enregistré ne peut pas renvoyer USER (ERR_ALREADYREGISTRED)"""
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((SERVER, PORT))
        
        sock.sendall(f"PASS {PASSWORD}\r\nNICK testuser4\r\nUSER testuser4 0 * :Real Name\r\n".encode())
        time.sleep(0.1)
        sock.recv(4096) # Vide le buffer de réception
        
        sock.sendall(b"USER testuser4 0 * :New Name\r\n")
        time.sleep(0.1)
        response = sock.recv(4096).decode()
        sock.close()
        
        self.assertIn("462", response, f"Le serveur n'a pas renvoyé l'erreur 462 pour USER. Réponse: {response}")

    def test_empty_nick(self):
        """Vérifie que la commande NICK échoue sans argument (ERR_NONICKNAMEGIVEN)"""
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((SERVER, PORT))
        
        sock.sendall(f"PASS {PASSWORD}\r\n".encode())
        sock.sendall(b"NICK\r\n") # Aucun argument fourni
        
        time.sleep(0.1)
        response = sock.recv(4096).decode()
        sock.close()
        
        self.assertIn("431", response, f"Le serveur n'a pas renvoyé 431 pour un NICK vide. Réponse: {response}")

    def test_wrong_password(self):
            """Vérifie que la connexion est refusée (ERR_PASSWDMISMATCH) et le socket fermé"""
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(2.0)
            sock.connect((SERVER, PORT))

            sock.sendall(b"PASS wrong_password\r\n")

            time.sleep(0.1)
            response = sock.recv(4096).decode()

            self.assertIn("464", response, f"Le serveur n'a pas renvoyé l'erreur 464. Réponse: {response}")

            time.sleep(0.1)
            try:
                rest = sock.recv(4096)
                self.assertEqual(rest, b"", "Le serveur n'a pas fermé la connexion après l'erreur 464.")
            except ConnectionError:
                pass
            except socket.timeout:
                # server doesn't close the socket
                self.fail("Timeout : Le serveur IRC n'a pas fermé la connexion réseau après le mauvais mot de passe.")
            finally:
                sock.close()


if __name__ == '__main__':
    unittest.main()