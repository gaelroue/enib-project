# Electronic-project
--------------------
This project is part of ENIB last year course. The goal is to work in team on 
developing a workflow control system with smart sensor.
 * Embedded Linux
 * Image processing
 * sensor communication
 * zigbee protocol
 * FPGA

### Installation :
```bash
$ git clone https://github.com/gaelroue/enib-project.git
$ cd _build
$ cmake .. && make
$ ./elec
```
###Coding Style
 * Do not use CamelCase like "ThisIsAVariable" prefer "this_is_a_variable".
 * Always use curly braces ({ and }) after if and else statements.  Follow
style of existing files:
	
	```
	if (foo) {
		do_bar();
	} else {
		do_baz();
	}
	```
 * Functions have the opening brace at the beginning of the next line, thus:

	```
	int function(int x)
	{
		body of function
	}
	```
 * In source files, separate functions with one blank line.
 * Declare pointers as `char * foo`, not `char* foo` or `char *foo`.
 * Use one space around (on each side of) most binary and ternary operators,
 such as any of these:

 =  +  -  <  >  *  /  %  |  &  ^  <=  >=  ==  !=  ?  :

 but no space after unary operators:

 &  *  +  -  ~  !  sizeof  typeof  alignof  __attribute__  defined

 no space before the postfix increment & decrement unary operators:

 ++  --

 no space after the prefix increment & decrement unary operators:

 ++  --

 and no space around the '.' and "->" structure member operators.

### Sensor refresh automatic :

For 1 second -> put 1000 in struct sensor refresh_time.
500ms -> put 500;
1,5s -> put 1500...

### TODO :

### Useful links :
* frame type : http://ftp1.digi.com/support/utilities/digi_apiframes.htm
* zibee protocol : https://github.com/digidotcom/xbee_ansic_library/
