Tag-Parser
==========

A basic tag parser that processes files such as HTML or XML.


This includes an interface that must be inherited from. The tag parsers are
designed to handle formats similar to XML and HTML. Note that although HTML tags
can be processed, CSS and Javascript may not be able to be processed.

See example.txt for an example of the expected format.


Please note that this is a parser, not an interpreter; however, an interpreter can be embedded into
the parser by passing the interpreter type as the first template parameter.
