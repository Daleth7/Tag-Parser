Tag-Parser
==========

A basic tag parser that processes files such as HTML or XML.


This includes an interface that must be inherited from. The tag parsers are
designed to handle e.g.

<MYMARKUPLANGUAGE>
  <THISTAG ATTR = 55 ATTR2 = "Foobar" Space ATTR = "Attributes with spaces in the name will be clumped together: 'SpaceATTR'.">
    This text is also accessible via the string() function.
  </THISTAG>
</MYMARKUPLANGUAGE>
<!-- This is a comment -->
<ANOTHERLANGUAGE>
  <!-- This is another comment -->
  <ONELINETAG ATTR = "One line tags are allowed if they end with '/'." />
  <BORINGTAG  />
</ANOTHERLANGUAGE>



Example use:

struct Parser_Interface : Tag_Parser<> {
  Parser_Interface()
    : Tag_Parser("file.my_markup_language")
  {}
};

Parser_Interface parser;
Parser_Interface::storage_type data;
parser.dump_to(data);
for(const auto& l : data){
    dest << "Language: " << l.first << '\n';
    for(const auto& p : l.second){
        dest << "|\t" << "- Tag: " << p.first << '\n';
        dest << "|\t\t" << "-- Attributes:\n";
        for(const auto& pi : p.second.first)
            dest << "|\t\t\t--- " << pi.first << " = " << pi.second << '\n';
        dest << "|\t\t" << "-- Wrapped Text: " << p.second.second << '\n';
    }
}



Output:


Language: anotherlanguage
|	- Tag: boringtag
|		-- Attributes:
|		-- Wrapped Text: 
|	- Tag: onelinetag
|		-- Attributes:
|			--- attr = one line tags are allowed if they end with '/'.
|		-- Wrapped Text: 
Language: mymarkuplanguage
|	- Tag: thistag
|		-- Attributes:
|			--- attr = 55
|			--- attr2 = foobar
|			--- spaceattr = attributes with spaces in the name will be clumped together: 'spaceattr'.
|		-- Wrapped Text: This text is also accessible via the string() function.
