# Jeu de la vie
Le Jeu de la Vie est un automate cellulaire fascinant qui a captivé de nombreux scientifiques et chercheurs. Cet automate, proposé pour la première fois en 1970 par le mathématicien John Conway, repose sur des règles simples qui génèrent pourtant une complexité impressionnante. Le jeu s’appuie sur les principes suivants : une grille bidimensionnelle (x,y)  composée de cellules. Chaque cellule peut être dans l’un des deux états possibles : vivante ou morte. Le voisinage d’une cellule est constitué des 8 cellules adjacentes. Ces voisines influencent, à chaque génération, l’état des cellules de la grille, qu’elles soient vivantes ou mortes.
Pour que la grille évolue, deux règles fondamentales s’appliquent à chaque cellule à chaque génération :
	Une cellule morte, entourée exactement de trois voisines vivantes, devient vivante.

- Une cellule vivante, ayant deux ou trois voisines vivantes, reste vivante.
- Dans tous les autres cas, elle meurt.
  
Malgré cette simplicité, le Jeu de la Vie peut produire des comportements riches, parfois imprévisibles, allant de structures stables à des configurations dynamiques complexes, ce qui en fait un objet d’étude captivant.

 
# A.1) Spécifications 
L’objectif de ce projet est de développer, en binôme, une version complète et fonctionnelle du Jeu de la Vie en utilisant le langage de programmation C++ et les concepts de la programmation orientée objet (POO).

- Pour ce qui est de la partie affichage, notre jeu apparaitra soit dans la console de notre ordinateur, soit dans une interface graphique, en fonction de 2 modes que nous implémenterons. Concernant l’interface graphique, nous utiliserons la bibliothèque graphique SFML pour son implémentation et son développement.
- Pour ce qui est de la partie logique de notre programme, nous devrons implémenter une grille rectangulaire, d’une taille précisée dans un fichier pris en entrée, qui contiendra une matrice de booléens décrivant l’état des cellules (vivant : true | mort : false). L’exécution de notre programme se termine lorsque notre modèle n’évolue plus ou après un nombre de générations fixé arbitrairement. 

# A.2) Extensions possibles 
Pour que notre programme soit optimisé, il doit être robuste et de qualité, tout en ayant la possibilité de rajouter les extensions suivantes, sans rendre le code non opérationnel.  

- Gestion d’une grille torique. Les cellules placées aux extrémités de la grille sont adjacentes ; en d’autres termes les cellules en colonne zéro sont voisines des cellules en colonne N-1 avec N le nombre de colonnes. Le principe est similaire en ligne.  

- Introduction de cellules obstacle. L’état des cellules obstacles n’évolue pas au cours de l’exécution. Ces dernières possèdent un état vivant ou mort. Modifiez votre code, sans altérer le fonctionnement de base.

- Placement de constructions préprogrammées sur la grille. Utiliser les touches du clavier pour intégrer de nouveaux motifs sur la grille lors de l’exécution.

- Paralléliser la génération des cellules. Modifiez votre algorithme pour paralléliser le traitement requis pour mettre à jour l’état des cellules sur la grille.
