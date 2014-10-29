/***********************************************************************************
    Filename: hal_digio.c

    Copyright 2007 Texas Instruments, Inc.
***********************************************************************************/

#include <stdint.h>

#include "isr_compat.h"

#include "mesh_if_defs.h"

static ISR_FUNC_PTR port1_isr_tbl[8] = {0};
static ISR_FUNC_PTR port2_isr_tbl[8] = {0};

//----------------------------------------------------------------------------------
//  DESCRIPTION:
//    Configure the pin as specified by p.
//----------------------------------------------------------------------------------
uint8_t halDigioConfig(const digioConfig *p)
{
    register volatile uint8_t *dir;
    register volatile uint8_t *out;
    register const uint8_t bitmask = p->pin_bm;

    // Sanity check
    if ((bitmask == 0 ) ||  (bitmask != (uint8_t) (1 << p->pin)))
        return(HAL_DIGIO_ERROR);

    switch(p->port)
    {
    case 1:
        P1SEL &= ~bitmask;
        out = &P1OUT;
        dir = &P1DIR;
        break;
    case 2:
        P2SEL &= ~bitmask;
        out = &P2OUT;
        dir = &P2DIR;
        break;
    case 3:
        P3SEL &= ~bitmask;
        out = &P3OUT;
        dir = &P3DIR;
        break;
    case 4:
        P4SEL &= ~bitmask;
        out = &P4OUT;
        dir = &P4DIR;
        break;
    case 5:
        P5SEL &= ~bitmask;
        out = &P5OUT;
        dir = &P5DIR;
        break;
    case 6:
        P6SEL &= ~bitmask;
        out = &P6OUT;
        dir = &P6DIR;
        break;
    default:
        return(HAL_DIGIO_ERROR);
    }

    if (p->dir == HAL_DIGIO_OUTPUT)
    {
        if (p->initval == 1)
            *out |= bitmask;
        else
            *out &= ~bitmask;
        *dir |= bitmask;
    }
    else // input
    {
        *out &= ~bitmask;
        *dir &= ~bitmask;
    }
    return (HAL_DIGIO_OK);
}

//----------------------------------------------------------------------------------
//  uint8 halDigioIntConnect(const digioConfig *p, ISR_FUNC_PTR func)
//----------------------------------------------------------------------------------
uint8_t halDigioIntConnect(const digioConfig *p, ISR_FUNC_PTR func)
{
    istate_t key;

    key = __get_interrupt_state();
    __disable_interrupt();
    switch (p->port)
    {
    case 1:
        port1_isr_tbl[p->pin] = func;
        break;
    case 2:
        port2_isr_tbl[p->pin] = func;
        break;
    default:
        __set_interrupt_state(key);
        return(HAL_DIGIO_ERROR);
    }
    halDigioIntClear(p);
    __set_interrupt_state(key);
    return HAL_DIGIO_OK;
}

//----------------------------------------------------------------------------------
//  uint8 halDigioIntEnable(const digioConfig *p)
//----------------------------------------------------------------------------------
uint8_t halDigioIntEnable(const digioConfig *p)
{
    switch (p->port)
    {
    case 1:
        P1IE |= p->pin_bm;
        break;
    case 2:
        P2IE |= p->pin_bm;
        break;
    default:
        return(HAL_DIGIO_ERROR);
    }
    return(HAL_DIGIO_OK);
}

//----------------------------------------------------------------------------------
//  uint8 halDigioIntDisable(const digioConfig *p)
//----------------------------------------------------------------------------------
uint8_t halDigioIntDisable(const digioConfig *p)
{
    switch (p->port)
    {
    case 1:
        P1IE &= ~p->pin_bm;
        break;
    case 2:
        P2IE &= ~p->pin_bm;
        break;
    default:
        return(HAL_DIGIO_ERROR);
    }
    return(HAL_DIGIO_OK);
}

//----------------------------------------------------------------------------------
//  uint8 halDigioIntClear(const digioConfig *p)
//----------------------------------------------------------------------------------
uint8_t halDigioIntClear(const digioConfig *p)
{
    switch (p->port)
    {
    case 1:
        P1IFG &= ~p->pin_bm;
        break;
    case 2:
        P2IFG &= ~p->pin_bm;
        break;
    default:
        return(HAL_DIGIO_ERROR);
    }
    return(HAL_DIGIO_OK);
}

//----------------------------------------------------------------------------------
//  uint8 halDigioIntSetEdge(const digioConfig *p, uint8 edge)
//----------------------------------------------------------------------------------
uint8_t halDigioIntSetEdge(const digioConfig *p, uint8_t edge)
{
    switch (edge)
    {
    case HAL_DIGIO_INT_FALLING_EDGE:
        switch(p->port)
        {
        case 1:
            P1IES |= p->pin_bm; break;
        case 2:
            P2IES |= p->pin_bm; break;
        default:
            return(HAL_DIGIO_ERROR);
        }
        break;
    case HAL_DIGIO_INT_RISING_EDGE:
        switch(p->port)
        {
        case 1:
            P1IES &= ~p->pin_bm; break;
        case 2:
            P2IES &= ~p->pin_bm; break;
        default:
            return(HAL_DIGIO_ERROR);
        }
        break;
    default:
        return(HAL_DIGIO_ERROR);
    }
    return(HAL_DIGIO_OK);
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
ISR(PORT1, port1_isr)
{
    register uint8_t i;

    if (P1IFG)
    {
        for (i = 0; i < 8; i++)
        {
            register const uint8_t pinmask = 1 << i;
            if ((P1IFG & pinmask) && (P1IE & pinmask) && (port1_isr_tbl[i] != 0))
            {
                (*port1_isr_tbl[i])();
                P1IFG &= ~pinmask;
            }
        }
        __low_power_mode_off_on_exit();
    }
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
ISR(PORT2, port2_isr)
{
    register uint8_t i;

    if (P2IFG)
    {
        for (i = 0; i < 8; i++)
        {
            register const uint8_t pinmask = 1 << i;
            if ((P2IFG & pinmask) && (P2IE & pinmask) && (port2_isr_tbl[i] != 0))
            {
                (*port2_isr_tbl[i])();
                P2IFG &= ~pinmask;
            }
        }
        __low_power_mode_off_on_exit();
    }
}
