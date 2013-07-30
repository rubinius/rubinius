---
layout: doc_fr
title: Comment Faire - Ecrire un Billet pour le Blog
previous: Ecrire des Benchmarks
previous_url: how-to/write-benchmarks
next: Ecrire de la Documentation
next_url: how-to/write-documentation
---

Le blog Rubinius utilise Jekyll et est intégré dans le site web et la
documentation. Nous encourageons et apprécions les billets écrits par
des invités concernant vos expériences en tant qu'utilisateur ou
développeur Rubinius.

Le format préféré pour les billets est le Markdown. Toutefois, si vous
avez un besoin particulier de formattage, le billet peut être écrit en
HTML directement.

Clonez le dépôt Rubinius afin de récupérer le site web et les billets.

    git clone https://github.com/rubinius/rubinius.git

Pour démarrer, assurez-vous d'avoir installé les gems `kramdown` et `jekyll`.

    rbx gem install jekyll kramdown

Allez ensuite dans le répertoire des billets

    cd rubinius/web/_posts

Créez un fichier par la console dont le nom suit la convention `AAAA-MM-JJ-perma-link.markdown`.

    touch "%(date +"%Y-%m-%d")-perma-link-title.markdown"

Ecrivez votre billet génial

    cat /dev/random > <<le billet en question>> # :-p

Lancez jekyll pour compiler le site web, qui incluera votre billet

    cd ../web/
    rbx -S jekyll build

Créez un commit pour tous vos changements dans le dossier `web/`

    cd ../
    git add web/
    git commit -m "Wrote a blog post on ....."

Soumettez un patch, une pull request, ou si vous avez les droits
d'écrire sur le dépôt, poussez sur la branche master.

Prévenez-nous qu'il y a un nouveau billet sur le blog. Nous pourrions
avoir des retour à vous soumettre avant de publier le billet.
