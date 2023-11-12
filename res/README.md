# Using Resources
## imports.cfg
to here you'll list all your resources!!  
if you're new and this seems confusing, check the CFG format section at the end! (this will still seem super silly but a bit less confusing!)  
this file contains an import list for each import type
### Import types and their directories:
- fonts: `/fonts`, ttfs and otfs
- models: `/models`, 3d models
- shaders: `/shaders`, glsl shaders, `/shaders/standard` contains all the shaders required for the core, please don't write the game-specific or modified ones there
- stages: `/stages`, see stage serialization
- textures: `/textures`, for the funny cat jpgs
### import field types
fonts, models and textures use the general `STRUCT {req}STRING path; {opt}STRING id; {opt} additional...`. The path is from the corresponding directory. If the id isn't specified, it will become the same as the path. As a good practice you'd want to explicitly identify each one, but for some reason I like to keep textures implicitly identified. PNG and JPG paths have something special y'know.  
The additional data is all the following optional struct members that can be used for specifying optional extra-data about the resource. The only one that uses it so far is the font import, with the first additional field the resolution can be overwritten (default 48px)  
Now the fun exceptions:
- stages: `STRING path`: the id is always specified in the stage file itself.
- shaders: `STRUCT {req}STRING id; {req}STRING vertexPath; {req}STRING fragmentPath; {req}STRING geometryPath` yeah, shaders need multiple files to work, at least the fragment and the vertex shader. Note that the id has to be first!

## materials.json
todo actually write this
## objects.json
this one too

## CFG syntax
it's yaml but broken!!! :3 silly little file format  
ett exempel  
```
# a comment
string: "haii!!!"
string2: PASCAL_CASE_STRING

num: 5.0

struct: "1" 2 alias

array:
  1
  2
  3

structarray:
  "struct" 1
  "struct2"
```
### comments
\# comment  
no multiline comments yet
### whitespace
whitespace count doesn't matter, you can also use whatever indentation as long as it stays the same.
### field types
define a field with `[name]: [value]`  
a field name can contain alphabets, numerals and underscores. though it can't start with a numeral
### string  
a string can be written with either 'commas' or "these guys"! Also if the string contains only alphabets, numbers or underscores, you can just write it as it is (if it would be fine as a field name, it should be fine as a string too)
### number
pretty straightforward.  
for example 865 is an integer and -7.342 is a floating point decimal. Integers can be casted to floating points but not the other way around, see parsing for more.
### struct
structs can have multiple members! we'll look to imports.cfg for example:  
`AREA_TEXT "standard/text.vert" "areatext.frag"`  
so what's going on????  
this is a shader struct with 3 members, every one of them is a string. we'll call this struct a [STRING, STRING, STRING]. (the members are separated by spaces, pretty obvious)  
structs can have multiple differrent types, however the next type,
### array,
can't.  
define an array with
```
[name]:
  [member1]
  [member2]
  [member3]
  ...
```
so, first the name, then an underscore, then all the elements with same indentation.  
arrays can be of one type only, but that type can be anything (structs are ok, pretty sure subarrays are also allowed - holdonletmecheck, yup, they're fine too). Note that array elements can also have names, though I haven't used them even once.

## CFG parsing and validation
lessgoo, this is the cool stuff!!!  
so, cfg parsing has two stages, the parsing and validation
### Parsing stage
basically serializing the file. not much to say here.  
### Validation stage
a fluent cfg user writes a template for a file! then the file gets checked (if it fits the template) and some data can be even added or modifier to fit the template!  
here's a video settings example template:
```
(C++)
{
    ...
    Mandatory("saturation", CFGFieldType::FLOAT),
    Mandatory("use_vsync", CFGFieldType::INTEGER),
    Mandatory("resolution", CFG_VEC2(CFGFieldType::INTEGER)),
    ...
}
```
so whats going on again????  
a field can be mandatory or optional, mandatory fields are absolutely required, otherwise the validation fails. Missing optional fields are added with default (or null) values at the validation stage.  
then the field name  
and then the type.  
the field type must match the one in the template. For most cases this is trivial, but with integers and floating points it can get a bit confusing. 456.654 for example will get parsed as a float and 456 as an integer. But if the template requires a float and a integer is given, the integer will get cast into a float. A float can't cast  into a string since I don't want to bother with rounding and such. But if you don't want to cast integers into floats implicitly, you can also use `CFGFieldType::NUMBER`, which will accept either one, but won't convert to float. Then you can check which type was given later!
  
CFG_VEC2 is actually just a macro:
```
#define CFG_VEC2(type) CFG_STRUCT(CFG_REQUIRE(type), CFG_REQUIRE(type))
```
oh, struct members can too be optional (default) or required.  
another example struct:
```
#define CFG_IMPORT CFG_REQUIRE(CFG::CFGFieldType::STRING), CFG::CFGFieldType::STRING
```
and here's how it looks in practice:  
example object import: `"gate.obj" OBJ_GATE`  
example texture import: `"wood.jpg"`  
see, the first one has both fields, the second one just the required one.  
now, it's up to the programmers imagination how the data will be used, but in this example the first struct element represents the path and the second the resource ID. Each resource must have an ID but if one isn't specified explicitly, it will just be the same as the path.  
hope this nonsense makes sense!