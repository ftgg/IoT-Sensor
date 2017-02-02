
#ifndef SPI_H_
#define SPI_H_

/*
 * Initialize spi interface
 */
void init_spi_registers(char interface_id);

/*
 * Writes a chraracter
 */
void spi_putc(char interface_id, char data);

/*
 * Returns a character
 */
char spi_getc(char interface_id);

/*
 * Write length bytes from buffer
 */
int spi_write(char interface_id, char * buffer, int length);


/*
 * Read lenght characters and writes them into buffer
 */
int spi_read(char interface_id, char * buffer, int length);



#endif /* SPI_H_ */
