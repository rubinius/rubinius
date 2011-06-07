---
layout: doc_fr
title: Comment Faire - Ecrire un Ticket
previous: Comment Faire
previous_url: how-to
next: Ecrire un Test Ruby
next_url: how-to/write-a-ruby-spec
---

Le gestionnaire de ticket Rubinius se trouve à l'adresse <http://github.com/rubinius/rubinius/issues>.

Pour être utile, un ticket doit être concis, précis et permettre d'agir.
S'il ne l'est pas, le ticket va engendrer des aller-tours, des
discussions, et n'avancera pas. De la même manière, les tickets doivent
appartir aux catégories ci-dessous:

  1. Un historique précis de ligne de commande montrant comment
     installer, invoquer le programme et afficher une trace d'exécution
     pour une exception.
  2. Un bref bout de code illustrant le problème et la ligne de commande
     pour le provoquer.
  3. Un patch, **avec les tests adéquats**, montrant que les tests
     sont toujours valides.

Si votre problème ne rentrent pas dans une des catégories, il n'est pas
invalide. Pour autant créer un ticket n'est pas approprié.

  1. S'il s'agit d'une fonctionnalité, venez en discuter sur la
     mailing-list. De plus, vous pourriez également en commencer
     l'implémentation afin de démontrer que votre fonctionnalité est utile.
  2. S'il s'agit d'une librairie ou d'un gem non fonctionnel, prenez le
     temps d'investiguer pour tenter de reproduire le bug et donc, de
     créer un ticket.


## Procédure générale pour soumettre un ticket

  1. Vérifiez et revérifiez.

     1. Recompiler entièrement ('rake clean; rake') après 'git pull', ou après avoir clôné.
     2. Lisez la section [Dépannage](/doc/fr/getting-started/troubleshooting) pour vérifier si vous trouvez une solution au problème.
     3. Lisez [Tests](/doc/fr/specs/).

  2. Donnez un nom spécifique - court de préférence, à votre ticket.

  3. Tagguez votre ticket de manière appropriée.

  4. Donnez assez de détails à proopos du problème.

     * La ligne de commande qui invoque le programme
     * La trace d'exécution ou la différence entre le résultat attendu et le résultat actuel.
     * Des informations sur votre machine. `uname -a` suffit en général
       (si toutefois il y a des champs marqués "unknown" dans le
        résultat de cette commande, merci de nous décrire pourquoi).

  5. Rédigez le ticket en anglais.


## Instructions additionelle pour les tickets avec patchs

  *  Ils peuvent n'être que des suites de tests.
  *  Les patchs doivent être accompagnés de tests sauf si ces tests existent déjà.
  *  Les parties intéressantes des résultats de test et les invocations
     exactes des 'bin/mspec' à lancer, cela vaut pour les tests
     existants comme pour les nouveaux tests.
  *  Des descriptions additionnelles sur votre patch et de comment il corrige le problème.
     Veuillez indiquer également, en particulier pour les nouvelles
     fonctionnalités, si le sujet a déjà été discuté sur #rubinius ou ruby-dev.

Sauf si réellement impossible, veuillez utiliser 'git-format-patch' pour
créer vos ensembles de patchs. Cela les rend plus facile à appliquer et
préserve les attributions.
Sinon, fournissez un diff.


## Exemple de soumission de patch

Supposons que la suite de test suivante existe et qu'elle soit en échec:

    describe "Kernel.format" do
      it "is accessible as a module function" do
        Kernel.format("%s", "hello").should == "hello"
      end
    end

1. Titre du ticket:

   "[PATCH] No method 'format' on Kernel (Module)"

2. Etiquettes / Tags:

   "patch core spec"

3. Corps du ticket:

   The method 'format' is not available as a module function of Kernel.

       $ bin/mspec spec/ruby/core/kernel/format_spec.rb
       Started
       .E

       1)
       Kernel.format is accessible as a module function ERROR
       No method 'format' on Kernel (Module):

   The method 'format' already exists but has not been set as a module
   function.  This patch does so.

   After the patch is applied:

       $ bin/mspec spec/ruby/core/kernel/format_spec.rb
       Started
       ..

       Finished in 0.016031 seconds

       2 examples, 2 expectations, 0 failures, 0 errors

4. Pièces jointes:

Enfin, mettez votre patch dans un gist et ajoutez le lien vers le gist
dans votre ticket.

    From c61cecce6442347ebbdf1ded7a5c0832c97582c1 Mon Sep 17 00:00:00 2001
    From: Brian Ford <bford@engineyard.com>
    Date: Sat, 19 Jan 2008 17:48:19 -0800
    Subject: [PATCH] Set Kernel#format as a module function.


    diff --git a/kernel/core/kernel.rb b/kernel/core/kernel.rb
    index 2d2e508..f2a382e 100644
    --- a/kernel/core/kernel.rb
    +++ b/kernel/core/kernel.rb
    @@ -150,6 +150,7 @@ module Kernel
       end
       alias_method :format, :sprintf
       module_function :sprintf
    +  module_function :format
       module_function :abort

       def puts(*a)

