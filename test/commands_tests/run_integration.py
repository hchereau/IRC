import unittest
import sys

# On surcharge l'affichage classique pour l'aérer
class SpacedTestResult(unittest.TextTestResult):
    def startTest(self, test):
        super().startTest(test)
        
    def addSuccess(self, test):
        super().addSuccess(test)
        self.stream.write("\n") # Ajoute une ligne vide si succès

    def addFailure(self, test, err):
        super().addFailure(test, err)
        self.stream.write("\n") # Ajoute une ligne vide si échec

class SpacedTestRunner(unittest.TextTestRunner):
    resultclass = SpacedTestResult

if __name__ == '__main__':
    # Découverte automatique des tests
    suite = unittest.TestLoader().discover('test/commands_tests/', pattern='test_*.py')
    
    # Lancement avec notre affichage aéré
    result = SpacedTestRunner(verbosity=2).run(suite)
    
    # Quitte avec une erreur (1) si un test a échoué (pour la CI)
    sys.exit(not result.wasSuccessful())