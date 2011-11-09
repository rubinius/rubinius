---
layout: doc_es
title: Cómo - enviar cambios a Github
previous: Translate Documentation
previous_url: how-to/translate-documentation
next: Appendix A - Glossary
next_url: appendix-a-glossary
---

El proyecto Rubinius hace su trabajo en su mayoría en la rama master.
El objetivo principal es mantener "limpio" de modo que siempre se construya
el binario y ofrece un snapshot de lás últimas correciones y mejoras.

### Committers - Acceso con acceso de lectura/escritura del repositorio de Rubinius

Le recomendamos a los committers quiénes tienen acceso de lectura/escritura
al repositorio hagan su trabajo en una rama desde su repositorio local.
Como los cambios deben ser estables, estos deben seguir los dos pasos.
El primer paso es incluir la especificación que destaca el comportamiento en
construcción, mientras que el segundo añade commit agrega y permite a la
especificación pasar.

Después de hacer los cambios en su rama desde el repositorio local, el commit
debe ser unido denuevo a la rama principal y ser enviarlo desde github.
Para evitar mensajes de git merge, le pedimos primero haga git rebase a la
rama principal antes de la fusión (merge).


1. git branch name-of-fix-branch
2. git checkout name-of-fix-branch
3. <escriba la especificación>
4. <escriba código que pase la especificación>
5. git add <list of spec files>
6. git commit
7. git add <liste los archivos>
8. git commit
9. git checkout master
10. git pull --rebase
11. git checkout name-of-fix-branch
12. git rebase master
13. git checkout master
14. git merge name-of-fix-branch
15. git push origin master

Pasos de 9 a 15 se pueden automatizar vía un script que guarde todos
lo que escribimos.

### Committers con acceso de sólo-lectura al repositorio de Rubinius

Por favor, lea:

  *  [Cómo escribir un ticket](/doc/es/how-to/write-a-ticket)

