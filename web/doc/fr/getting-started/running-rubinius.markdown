---
layout: doc_fr
title: Exécuter Rubinius
previous: Compilation
previous_url: getting-started/building
next: Dépannage
next_url: getting-started/troubleshooting
---

Une fois que vous avez suivi les étapes pour compiler (et optionnellement installer)
Rubinius, vous pouvez vérifier son fonctionnement :

    rbx -v

Rubinius fonctionne en général comme Ruby sur la ligne de commande. Par exemple :

    rbx -e 'puts "Bonjour !"'

Pour exécuter un fichier ruby nommé 'code.rb' :

    rbx code.rb

Pour exécuter IRB :

    rbx

Si vous avez ajouté le répertoire _bin_ de Rubinius à votre `PATH`,
Rubinius devrait se comporter exactement comme vous l'attendriez de MRI.
Vous avez accès aux commandes `ruby`, `rake`, `gem`, `irb`, `ri` et `rdoc`.

Vous pouvez ajouter le répertoire _bin_ de Rubinius à votre `PATH` seulement lorsque
vous désirez utiliser Rubinius. Ainsi, il n'interférera pas avec votre installation
standard de Ruby lorsque vous ne voulez pas utiliser Rubinius.

