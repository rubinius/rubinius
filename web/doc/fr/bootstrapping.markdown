---
layout: doc_en
title: Bootstrapping
previous: Build System
previous_url: build-system
next: Virtual Machine
next_url: virtual-machine
---

L'amorçage (bootstrapping) est le process de mise en place de l'ensemble des 
fonctions jusqu'à ce que du code Ruby puisse être exécuté. L'amorçage comporte
7 étapes:

  1. VM: La machine virtuelle est capable de charger et d'éxécuter du
     bytecode, d'envoyer des messages (recherche et exécution de méthodes), et
     toutes les  fonctions primitives sont disponibles, mais pas sous forme de
     méthode Ruby.
  
     La classe Class doit être mise en place manuellement dès cette étape, 
     en configurant sa classe : elle-même et sa super-class: Module. En plus
     de Class et Module d'autres classes de base sont créées notamment Object,
     Tuple, LookupTable et MethodTable.
     
     Dès lors que des classes peuvent être définies, environs 35 classes sont 
     auto-initialiées, de même des symboles pour les méthodes de premier
     niveau (:object_id, :call, :protected, etc) sont créés, les exceptions
     basiques sont définies et les primitives sont enregistrées. Les
     entrées-sorties sont mises en place. Enfin à cette étape, plusieurs
     méthodes Ruby fondamentales sont reliées à des fonctions primitives.
     
     A ce niveau, les comportements définis sont sufissant pour démarrrer le
     chargement du reste du noyau d'exécution entièrement défini en Ruby.
     Ce chargement demande de plus en plus d'étapes au fur et à mesure que le
     language grandit.
     
  2. alpha: Cette étape démarre le chargement de code Ruby. Il est possible
     de réouvrir les classes et modules et de définir des méthodes. Le minimum
     de fonctionnalités pour permettre l'implémentation des méthodes suivantes
     est implémentée dans kernel/alpha.rb :

       attr_reader :sym
       attr_writer :sym
       attr_accessor :sym
       private :sym
       protected :sym
       module_function :sym
       include mod

     Il est aussi possible de lever des exceptions et de terminer le processus
     courant. Cette étape est la fondation des deux suivantes.

  3. boostrap: Cette étape continue d'ajouter les fonctionnalitées minimums
     pour le support du chargement de la plateforme. Les fonctions primitives
     sont ajoutés pour la plupart des classes noyaux.
     
  4. platform: Le système FFI (inteface pour les fonctions externes ) est
     implémanté et les méthodes d'interface Ruby spécifique à une plateforme
     sont créés. Une fois mis en place, ce qui est spécifique à une plateforme
     est attaché (pointeurs, accès aux fichiers, math et commandes POSIX)

  5. common: La vaste majorité de la librairie principale est implémentée.
     Les classes sont gardées avec une implémentation aussi neutre que
     possible. De plus la plupart des fonctionnalitées spécifiques à Rubinius
     sont ajoutées.
     
  6. delta: La version finale des méthodes du type #attr_reader sont ajoutés.
     Les implémentations spécifiques à une plateforme des méthodes surchagent
     celles fournies à l'étape common.

  7. loader: La version compilé de kernel/loader.rb est exécutée.
    
     L'étape finale met en place le cycle de vie d'un process ruby. Elle
     commence par connecter la VM au système, mettre en place les chemins
     de chargement, et en lire les scripts de personnalisation du répertoire
     home. Elle retient les signaux et traite les arguments de la ligne de
     commande.
     
     Ensuite, soit le script passé en ligne de commande est exécuté, soit un
     shell interactif ruby est démarré. Une fois fini, chaque bloc at_exit
     enregistré est exécuté, tout les objets sont finalisés et le système se
     termine.
     

## Ordre de chargement

Les fichiers dans les répértoires noyaux: boostrap, platform, common et delta 
implémentent les étapes d'amorçage ci-dessus. L'ordre de chargement de ces
répertoires est spécifié dans le fichier runtime/index.

Lorsqu'un fichier rbc est chargé, le code au niveau du script et dans les
corps de class ou module est exécuté. Par exemple, lors du chargemnet de :

    class SomeClass
      attr_accessor :value
    end

l'appel à #attr_accessor sera exécuté. Cela demande que le code appelé dans
les corps de script, class ou module soit chargé avant le fichier qui appel
ce code. Le fichier kernel/alpha.rb définit la plupart du code qui sera
nécessaire au niveau du script ou du module. Cependant, d'autre dépendances
de l'ordre du charement existent entre certains des fichiers de platform,
common, delta et du compilateur.

Ces ordres de chargement sont gérés par le fichier load_order.txt, situé dans
chacun des répertoires kernel/\*\*. Si vous modifiez du code qui ajoute
une dépendance à l'ordre de chargement, vous devez éditer le fichier
load_order.txt en ajoutant le fichier dont vous dépendez au-dessus du fichier
qui en dépend. De plus , si vous ajoutez un nouveau fichier à l'un des répertoires noyaux, vous devez ajouter le nom du fichier au fichier
load_order.txt de ce répertoire. Ces fichiers sont copiés dans les répértoires runtime/\*\* correspondant pendant la phase de construction. A chaque étape de l'amorçage ci dessus, la VM charge les fichiers listé dans load_order.txt en respectant leur ordre.
