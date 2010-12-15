---
layout: doc_es
title: How-To - Translate Documentation
previous: Write Documentation
previous_url: how-to/write-documentation
next: Appendix A - Glossary
next_url: appendix-a-glossary
translated: true
---

Hay dos tareas de traducción:

1. Actualizar una traducción existente
1. la creación de una traducción completamente nueva

Comience por leer [Cómo se hace - Escribir
Documentación] (/doc/es/how-to/write-documentation/)


### Actualizar una Traducción Existente

Para actualizar una traducción existente, abra el archivo de tema en
`web/doc/LANG` y editar el texto existente o añadir texto recién traducido.


### Creación de una Nueva Traducción

Para crear una nueva traducción de la lengua:

1. Copia `web/doc/en` a `web/doc/LANG` donde _LANG_ es la norma ISO-639-2
   código para la traducción de la lengua que está creando.
1. Editar los enlaces en la tabla de contenido para que apunte a la ubicación
   de la archivos para la traducción. (Nota que por alguna razón el
   `page.base_dir` atributo no está disponible cuando estos archivos están
   siendo prestados por Jekyll.  Esto debe ser investigado.)
1. Traducir el texto en Inglés de la lengua meta.
