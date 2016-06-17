#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils/uartstdio.h"
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/tm4c123gh6pm.h"

#define SAMPLES 60
#define NUM_EPOCHS 3
#define PATH "/data/data"

/* A utility function to reverse a string  */
void reverse(char str[], int length) {
	int start = 0;
	int end = length - 1;
	while (start < end) {
		char temp = str[start];
		str[start] = str[end];
		str[end] = temp;
		start++;
		end--;
	}
}

// Implementation of itoa()
char* itoa(int num, char* str, int base) {
	int i = 0;
	bool isNegative = false;

	/* Handle 0 explicitely, otherwise empty string is printed for 0 */
	if (num == 0) {
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	// In standard itoa(), negative numbers are handled only with
	// base 10. Otherwise numbers are considered unsigned.
	if (num < 0 && base == 10) {
		isNegative = true;
		num = -num;
	}

	// Process individual digits
	while (num != 0) {
		int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	// If number is negative, append '-'
	if (isNegative)
		str[i++] = '-';

	str[i] = '\0'; // Append string terminator

	// Reverse the string
	reverse(str, i);

	return str;
}

int openOutputFiles(FILE *fp[NUM_EPOCHS]) {
	FILE *metadata;
	char fileNames[NUM_EPOCHS][100];
	int i, old_files;

	// open metadata file to read number of previously created files
	metadata = fopen("/data/metaData.txt", "r");

	// seek to the end of file to check for empty file
	if (fseek(metadata, 0, SEEK_END) == 0) {
		if (ftell(metadata) == 0) {
			old_files = 0;
		} else {
			rewind(metadata);
			fscanf(metadata, "%d", &old_files);
		}
	} else {
		rewind(metadata);
		old_files = 0;
	}
	fclose(metadata);

	for (i = 0; i < NUM_EPOCHS; i++) {
		char buff[100];
		itoa(old_files + i, buff, 10);
		strcpy(fileNames[i], PATH);
		strcat(fileNames[i], buff);
		strcat(fileNames[i], ".csv");
	}

	for (i = 0; i < NUM_EPOCHS; i++) {
		fp[i] = fopen(fileNames[i], "w");
	}

	return old_files;
}

void printDataToFiles(FILE *fp[NUM_EPOCHS], uint32_t data[NUM_EPOCHS][SAMPLES]) {
	int i, j, old_files;
	FILE *metadata;

	old_files = openOutputFiles(fp);
	for (i = 0; i < NUM_EPOCHS; i++) {
		for (j = 0; j < SAMPLES; j++) {
			fprintf(fp[i], "%d,", data[i][j]);
		}
		fclose(fp[i]);
//		free(fp[i]);
	}

	// open metadata file to read number of previously created files
	metadata = fopen("/data/metaData.txt", "w");

	fprintf(metadata, "%d", (old_files + NUM_EPOCHS));
	fclose(metadata);
//	free(metadata);
}
//*****************************************************************************
//
// Configure ADC0 for a single-ended input and a single sample.  Once the
// sample is ready, an interrupt flag will be set.  Using a polling method,
// the data will be read then stored in a file.
//
//*****************************************************************************
int main(void) {

	/**
	 * File pointer for writing analog values to "sensor_data.txt" file in
	 * csv format.
	 * Open/Create output file as write only, clears previous data in the
	 * file if it exists.
	 */
	FILE *fp[NUM_EPOCHS];

#if defined(TARGET_IS_TM4C129_RA0) ||                                         \
    defined(TARGET_IS_TM4C129_RA1) ||                                         \
    defined(TARGET_IS_TM4C129_RA2)
	uint32_t ui32SysClock;
#endif

//
// This array is used for storing the data read from the ADC FIFO. It
// must be as large as the FIFO for the sequencer in use.  This example
// uses sequence 3 which has a FIFO depth of 1.  If another sequence
// was used with a deeper FIFO, then the array size must be changed.
//
	uint32_t pui32ADC0Value[1];

//
// Set the clocking to run at 20 MHz (200 MHz / 10) using the PLL.  When
// using the ADC, you must either use the PLL or supply a 16 MHz clock
// source.
// TODO: The SYSCTL_XTAL_ value must be changed to match the value of the
// crystal on your board.
//
#if defined(TARGET_IS_TM4C129_RA0) ||                                         \
    defined(TARGET_IS_TM4C129_RA1) ||                                         \
    defined(TARGET_IS_TM4C129_RA2)
	ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
					SYSCTL_OSC_MAIN |
					SYSCTL_USE_PLL |
					SYSCTL_CFG_VCO_480), 20000000);
#else
	SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
	SYSCTL_XTAL_16MHZ);
#endif

//
// The ADC0 peripheral must be enabled for use.
//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

//
// For this example ADC0 is used with AIN0 on port E3.
// The actual port and pins used may be different on your part, consult
// the data sheet for more information.  GPIO port E needs to be enabled
// so these pins can be used.
// TODO: change this to whichever GPIO port you are using.
//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

//
// Select the analog ADC function for these pins.
// Consult the data sheet to see which functions are allocated per pin.
// TODO: change this to select the port/pin you are using.
//
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

//
// Enable sample sequence 3 with a processor signal trigger.  Sequence 3
// will do a single sample when the processor sends a signal to start the
// conversion.  Each ADC module has 4 programmable sequences, sequence 0
// to sequence 3.  This example is arbitrarily using sequence 3.
//
	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

//
// Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
// single-ended mode (default) and configure the interrupt flag
// (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
// that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
// 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
// sequence 0 has 8 programmable steps.  Since we are only doing a single
// conversion using sequence 3 we will only configure step 0.  For more
// information on the ADC sequences and steps, reference the datasheet.
//
	ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
	ADC_CTL_END);

//
// Since sample sequence 3 is now configured, it must be enabled.
//
	ADCSequenceEnable(ADC0_BASE, 3);

//
// Clear the interrupt status flag.  This is done to make sure the
// interrupt flag is cleared before we sample.
//
	ADCIntClear(ADC0_BASE, 3);

	int loop = 0;
	int num_epoch = 0;
	uint32_t data[NUM_EPOCHS][SAMPLES];
//
// Sample AIN0 for loop. Store the value in the file.
//
	while (num_epoch < NUM_EPOCHS) {
		while (loop < SAMPLES) {
			// loop infinitely
//			loop = 0;

//
// Trigger the ADC conversion.
//
			ADCProcessorTrigger(ADC0_BASE, 3);

			//
			// Wait for conversion to be completed.
			//
			while (!ADCIntStatus(ADC0_BASE, 3, false)) {
			}

			//
			// Clear the ADC interrupt flag.
			//
			ADCIntClear(ADC0_BASE, 3);

			//
			// Read ADC Value.
			//
			ADCSequenceDataGet(ADC0_BASE, 3, pui32ADC0Value);

			//
			// Write the AIN0 (PE3) digital value data buffer.
			//
			data[num_epoch][loop] = pui32ADC0Value[0];
			loop++;

			//
			// This function provides a means of generating a constant length
			// delay.  The function delay (in cycles) = 3 * parameter.  Delay of /12 is
			// 250ms arbitrarily.
			//
#if defined(TARGET_IS_TM4C129_RA0) ||                                         \
    defined(TARGET_IS_TM4C129_RA1) ||                                         \
    defined(TARGET_IS_TM4C129_RA2)
			SysCtlDelay(ui32SysClock / 96);
#else
			SysCtlDelay(SysCtlClockGet() / 96);
#endif
		}
		num_epoch++;
		loop = 0;
	}
	printDataToFiles(fp, data);
}
