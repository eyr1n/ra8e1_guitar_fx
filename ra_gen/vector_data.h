/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
#ifdef __cplusplus
        extern "C" {
        #endif
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (8)
#endif
/* ISR prototypes */
void iic_master_rxi_isr(void);
void iic_master_txi_isr(void);
void iic_master_tei_isr(void);
void iic_master_eri_isr(void);
void ssi_txi_isr(void);
void ssi_rxi_isr(void);
void ssi_int_isr(void);
void gpt_counter_overflow_isr(void);

/* Vector table allocations */
#define VECTOR_NUMBER_IIC1_RXI ((IRQn_Type) 0) /* IIC1 RXI (Receive data full) */
#define IIC1_RXI_IRQn          ((IRQn_Type) 0) /* IIC1 RXI (Receive data full) */
#define VECTOR_NUMBER_IIC1_TXI ((IRQn_Type) 1) /* IIC1 TXI (Transmit data empty) */
#define IIC1_TXI_IRQn          ((IRQn_Type) 1) /* IIC1 TXI (Transmit data empty) */
#define VECTOR_NUMBER_IIC1_TEI ((IRQn_Type) 2) /* IIC1 TEI (Transmit end) */
#define IIC1_TEI_IRQn          ((IRQn_Type) 2) /* IIC1 TEI (Transmit end) */
#define VECTOR_NUMBER_IIC1_ERI ((IRQn_Type) 3) /* IIC1 ERI (Transfer error) */
#define IIC1_ERI_IRQn          ((IRQn_Type) 3) /* IIC1 ERI (Transfer error) */
#define VECTOR_NUMBER_SSI0_TXI ((IRQn_Type) 4) /* SSI0 TXI (Transmit data empty) */
#define SSI0_TXI_IRQn          ((IRQn_Type) 4) /* SSI0 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SSI0_RXI ((IRQn_Type) 5) /* SSI0 RXI (Receive data full) */
#define SSI0_RXI_IRQn          ((IRQn_Type) 5) /* SSI0 RXI (Receive data full) */
#define VECTOR_NUMBER_SSI0_INT ((IRQn_Type) 6) /* SSI0 INT (Error interrupt) */
#define SSI0_INT_IRQn          ((IRQn_Type) 6) /* SSI0 INT (Error interrupt) */
#define VECTOR_NUMBER_GPT0_COUNTER_OVERFLOW ((IRQn_Type) 7) /* GPT0 COUNTER OVERFLOW (Overflow) */
#define GPT0_COUNTER_OVERFLOW_IRQn          ((IRQn_Type) 7) /* GPT0 COUNTER OVERFLOW (Overflow) */
/* The number of entries required for the ICU vector table. */
#define BSP_ICU_VECTOR_NUM_ENTRIES (8)

#ifdef __cplusplus
        }
        #endif
#endif /* VECTOR_DATA_H */
