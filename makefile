COMPILER = avr-gcc                       #Compiler that we are using is avr-gcc
MCU = atmega328p                         #The Micro-controller used is Atmega328p
PORT=/dev/ttyUSB0                        #The port used is USB0
OBJCOPY = avr-objcopy                    #avr-objcopy is used to copy data between two object files
CFLAGS = -Os                             # -0s optimises the memory
programmer = arduino                     #the programmer used is arduino
BAUDRATE = 57600                         #baudrate is 57600. It is the number of signal or symbol changes that occur per second
elf_file = adc.elf	                 #This is the variable created to store the elf file
c_file = adc.c                           #This is the variable created to store the c file
hex_file = adc.hex              	 # It is the variable created to store the hex file
objs = adc.elf adc.hex 			 # This variable stores all the object files that will be created during the execution of code

program: compile
	avrdude -F -V -c $(programmer) -p $(MCU) -P $(PORT) -b $(BAUDRATE) -U flash:w:$(hex_file) # Command to upload the hex file to MCU

compile: elf
	$(OBJCOPY) -O ihex -R .eeprom $(elf_file) $(hex_file) # Command to create the hex file from the elf file

elf: $(c_file)
	$(COMPILER) -mmcu=$(MCU) -Wall $(CFLAGS) -o $(elf_file) $(c_file) #Command to create the elf file from the c file

clean:
	rm -f $(objs) # Deletes all the executables created from the Directory
