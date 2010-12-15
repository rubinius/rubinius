---
layout: doc_en
title: How-To - Write Documentation
previous: Write a Blog Post
previous_url: how-to/write-a-blog-post
next: Translate Documentation
next_url: how-to/translate-documentation
---

La documentación Rubinius está integrado con el sitio web y blog. Utiliza
Jekyll al igual que los otros componentes.

Para empezar, asegúrese de tener la `kramdown` y `jekyll` gemas instalado.

     rbx gem install kramdown jekyll

La fuente de la documentación se encuentra en la `web/doc`. Hay subdirectorios
de cada idioma para que la documentación ha sido traducido (por ejemplo, `en`,
`es`, etc.)

Hay una tabla de contenido para cada traducción (por ejemplo,
`web/doc/es/index.markdown`). El resto de la documentación consiste en
archivos individuales que han YAML atributos para especificar cómo los
documentos son conectado. Esencialmente, la documentación puede ser visto como
un doblemente enlazada lista de los documentos con cada documento que apunta a
la anterior y siguiente documento. La Tabla de Contenidos documento muestra la
estructura completa.

El YAML atributos en una mirada documento como el siguiente:

    ---
    layout: doc_en
    title: How-To - Write Documentation
    previous: Write a Blog Post
    previous_url: how-to/write-a-blog-post
    next: Translate Documentation
    next_url: how-to/translate-documentation
    ---

El _layout_ especifica que Jekyll diseño a utilizar al formatear la documento.
El _layout_ debe ser `doc_LANG», donde _LANG_ es la norma ISO-639-2 código de
la lengua.

El _title_ especifica el título del documento que se muestra en la parte
superior de la página.

Los atributos _previous_ y _previous\_url_ dar el título y enlace a la
documento anterior. Del mismo modo, los atributos _next_ y _next\_url_ dar
la título y el vínculo para el próximo documento. Estos se utilizan para
mejorar la navegación documentación y limitando la cantidad de trabajo
necesario para re-order partes de la documentación.


### Editing Existing Documentation

Un primer esbozo de la documentación ha sido creado. Hay muchos temas que la
mera necesidad de contar con documentación escrita para ellos.

Para agregar la documentación de un tema existente, o para arreglar la
documentación existente, abrir el archivo para el tema de la sección
`web/doc/idioma` y añadir o mejorar el documentación.


### Adding New Documentation

To add documentation for which no existing topic exists:

1. Create a new file with the .markdown extension under `web/doc/LANG`.
1. Set up the attributes to link the new file into the existing files. This
   will require editing the _previous_ and _next_ attributes of the existing
   files to insert the new file.
1. To view your updates while you are working on them, run
   `rbx -S jekyll --server --auto`
1. Edit the new file using Markdown syntax.
1. In the `web/` directory, run `rbx -S jekyll`.
1. Commit all the changes in the `web/` directory.
