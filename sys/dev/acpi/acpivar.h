/*	$OpenBSD: acpivar.h,v 1.87 2017/04/08 01:20:10 deraadt Exp $	*/
/*
 * Copyright (c) 2005 Thorsten Lockert <tholo@sigmasoft.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _DEV_ACPI_ACPIVAR_H_
#define _DEV_ACPI_ACPIVAR_H_

#define ACPI_TRAMPOLINE		(19 * NBPG)
#define ACPI_TRAMP_DATA		(20 * NBPG)

#ifndef _ACPI_WAKECODE

#include <sys/timeout.h>
#include <sys/rwlock.h>
#include <machine/biosvar.h>

#include "acpipwrres.h"

/* #define ACPI_DEBUG */
#ifdef ACPI_DEBUG
extern int acpi_debug;
#define dprintf(x...)	  do { if (acpi_debug) printf(x); } while (0)
#define dnprintf(n,x...)  do { if (acpi_debug > (n)) printf(x); } while (0)
#else
#define dprintf(x...)
#define dnprintf(n,x...)
#endif

extern int acpi_hasprocfvs;

struct klist;
struct acpiec_softc;
struct acpipwrres_softc;

struct acpivideo_softc {
	struct device sc_dev;

	struct acpi_softc *sc_acpi;
	struct aml_node	*sc_devnode;
};

struct acpi_attach_args {
	char		*aaa_name;
	bus_space_tag_t	 aaa_iot;
	bus_space_tag_t	 aaa_memt;
	void		*aaa_table;
	struct aml_node *aaa_node;
	const char	*aaa_dev;
};

struct acpi_mem_map {
	vaddr_t		 baseva;
	u_int8_t	*va;
	size_t		 vsize;
	paddr_t		 pa;
};

struct acpi_q {
	SIMPLEQ_ENTRY(acpi_q)	 q_next;
	int			 q_id;
	void			*q_table;
	u_int8_t		 q_data[0];
};

struct acpi_taskq {
	SIMPLEQ_ENTRY(acpi_taskq)	next;
	void 				(*handler)(void *, int);
	void				*arg0;
	int				arg1;
};

struct acpi_wakeq {
	SIMPLEQ_ENTRY(acpi_wakeq)	 q_next;
	struct aml_node			*q_node;
	struct aml_value		*q_wakepkg;
	int				 q_gpe;
	int				 q_state;
};

#if NACPIPWRRES > 0
struct acpi_pwrres {
	SIMPLEQ_ENTRY(acpi_pwrres)	 p_next;
	struct aml_node			*p_node;	/* device's node */
	int				 p_state;	/* current state */

	int				 p_res_state;
	struct acpipwrres_softc		*p_res_sc;
};

typedef SIMPLEQ_HEAD(, acpi_pwrres) acpi_pwrreshead_t;
#endif /* NACPIPWRRES > 0 */

typedef SIMPLEQ_HEAD(, acpi_q) acpi_qhead_t;
typedef SIMPLEQ_HEAD(, acpi_wakeq) acpi_wakeqhead_t;

#define ACPIREG_PM1A_STS	0x00
#define ACPIREG_PM1A_EN		0x01
#define ACPIREG_PM1A_CNT	0x02
#define ACPIREG_PM1B_STS	0x03
#define ACPIREG_PM1B_EN		0x04
#define ACPIREG_PM1B_CNT	0x05
#define ACPIREG_PM2_CNT		0x06
#define ACPIREG_PM_TMR		0x07
#define ACPIREG_GPE0_STS	0x08
#define ACPIREG_GPE0_EN		0x09
#define ACPIREG_GPE1_STS	0x0A
#define ACPIREG_GPE1_EN		0x0B
#define ACPIREG_SMICMD		0x0C
#define ACPIREG_MAXREG		0x0D

/* Special registers */
#define ACPIREG_PM1_STS		0x0E
#define ACPIREG_PM1_EN		0x0F
#define ACPIREG_PM1_CNT		0x10
#define ACPIREG_GPE_STS		0x11
#define ACPIREG_GPE_EN		0x12

/* System status (_SST) codes */
#define ACPI_SST_INDICATOR_OFF	0
#define ACPI_SST_WORKING	1
#define ACPI_SST_WAKING		2
#define ACPI_SST_SLEEPING	3
#define ACPI_SST_SLEEP_CONTEXT	4

struct acpi_parsestate {
	u_int8_t		*start;
	u_int8_t		*end;
	u_int8_t		*pos;
};

struct acpi_reg_map {
	bus_space_handle_t	ioh;
	int			addr;
	int			size;
	int			access;
	const char		*name;
};

struct acpi_thread {
	struct acpi_softc   *sc;
	volatile int	    running;
};

struct acpi_mutex {
	struct rwlock		amt_lock;
#define ACPI_MTX_MAXNAME	5
	char			amt_name[ACPI_MTX_MAXNAME + 3]; /* only 4 used */
	int			amt_ref_count;
	int			amt_timeout;
	int			amt_synclevel;
};

struct gpe_block {
	int  (*handler)(struct acpi_softc *, int, void *);
	void *arg;
	int   active;
	int   edge;
};

struct acpi_devlist {
	struct aml_node			*dev_node;
	TAILQ_ENTRY(acpi_devlist)	dev_link;
};

TAILQ_HEAD(acpi_devlist_head, acpi_devlist);

struct acpi_ac {
	struct acpiac_softc	*aac_softc;
	SLIST_ENTRY(acpi_ac)	aac_link;
};

SLIST_HEAD(acpi_ac_head, acpi_ac);

struct acpi_bat {
	struct acpibat_softc	*aba_softc;
	SLIST_ENTRY(acpi_bat)	aba_link;
};

SLIST_HEAD(acpi_bat_head, acpi_bat);

struct acpi_sbs {
	struct acpisbs_softc	*asbs_softc;
	SLIST_ENTRY(acpi_sbs)	asbs_link;
};

SLIST_HEAD(acpi_sbs_head, acpi_sbs);

struct acpi_softc {
	struct device		sc_dev;

	bus_space_tag_t		sc_iot;
	bus_space_tag_t		sc_memt;
#if 0
	bus_space_tag_t		sc_pcit;
	bus_space_tag_t		sc_smbust;
#endif

	/*
	 * First-level ACPI tables
	 */
	struct acpi_fadt	*sc_fadt;
	acpi_qhead_t		 sc_tables;
	acpi_wakeqhead_t	 sc_wakedevs;
#if NACPIPWRRES > 0
	acpi_pwrreshead_t	 sc_pwrresdevs;
#endif /* NACPIPWRRES > 0 */
	int			 sc_hw_reduced;

	/*
	 * Second-level information from FADT
	 */
	struct acpi_facs	*sc_facs;	/* Shared with firmware! */

	struct klist		*sc_note;
	struct acpi_reg_map	sc_pmregs[ACPIREG_MAXREG];
	bus_space_handle_t	sc_ioh_pm1a_evt;

	void			*sc_interrupt;

	struct rwlock		sc_lck;

	struct {
		int slp_typa;
		int slp_typb;
	}			sc_sleeptype[6];
	int			sc_maxgpe;
	int			sc_lastgpe;

	struct gpe_block	*gpe_table;

	int			sc_threadwaiting;
	u_int32_t		sc_gpe_sts;
	u_int32_t		sc_gpe_en;
	struct acpi_thread	*sc_thread;

	struct aml_node		*sc_tts;
	struct aml_node		*sc_pts;
	struct aml_node		*sc_bfs;
	struct aml_node		*sc_gts;
	struct aml_node		*sc_sst;
	struct aml_node		*sc_wak;
	int			sc_state;
	struct acpiec_softc	*sc_ec;		/* XXX assume single EC */

	struct acpi_ac_head	sc_ac;
	struct acpi_bat_head	sc_bat;
	struct acpi_sbs_head	sc_sbs;
	int			sc_havesbs;

	struct timeout		sc_dev_timeout;

	int			sc_revision;

	int			sc_pse;		/* passive cooling enabled */

	int			sc_flags;
};

extern struct acpi_softc *acpi_softc;

#define	SCFLAG_OREAD	0x0000001
#define	SCFLAG_OWRITE	0x0000002
#define	SCFLAG_OPEN	(SCFLAG_OREAD|SCFLAG_OWRITE)

#define GPE_NONE  0x00
#define GPE_LEVEL 0x01
#define GPE_EDGE  0x02

struct acpi_table {
	int	offset;
	size_t	size;
	void	*table;
};

struct acpi_dev_rank {
	struct device	*dev;
	int		rank;
	TAILQ_ENTRY(acpi_dev_rank) link;
};

#define	ACPI_IOC_GETFACS	_IOR('A', 0, struct acpi_facs)
#define	ACPI_IOC_GETTABLE	_IOWR('A', 1, struct acpi_table)
#define ACPI_IOC_SETSLEEPSTATE	_IOW('A', 2, int)

#if defined(_KERNEL)
struct   acpi_gas;
int	 acpi_map_address(struct acpi_softc *, struct acpi_gas *, bus_addr_t, bus_size_t,
			  bus_space_handle_t *, bus_space_tag_t *);

int	 acpi_map(paddr_t, size_t, struct acpi_mem_map *);
void	 acpi_unmap(struct acpi_mem_map *);
int	 acpi_probe(struct device *, struct cfdata *, struct bios_attach_args *);
u_int	 acpi_checksum(const void *, size_t);
void	 acpi_attach_machdep(struct acpi_softc *);
int	 acpi_interrupt(void *);
void	 acpi_powerdown(void);
void	 acpi_reset(void);


#define ACPI_SLEEP_SUSPEND	0x01
#define ACPI_SLEEP_HIBERNATE	0x02

int	 acpi_sleep_state(struct acpi_softc *, int);
void	 acpi_sleep_clocks(struct acpi_softc *, int);
int	 acpi_sleep_cpu(struct acpi_softc *, int);
void	 acpi_sleep_mp(void);
void	 acpi_sleep_pm(struct acpi_softc *, int);
void	 acpi_resume_pm(struct acpi_softc *, int);
void	 acpi_resume_clocks(struct acpi_softc *);
void	 acpi_resume_cpu(struct acpi_softc *);
void	 acpi_resume_mp(void);
void	 acpi_sleep_walk(struct acpi_softc *, int);


#define ACPI_IOREAD 0
#define ACPI_IOWRITE 1

void acpi_wakeup(void *);

int acpi_gasio(struct acpi_softc *, int, int, uint64_t, int, int, void *);

int	acpi_set_gpehandler(struct acpi_softc *, int,
	    int (*)(struct acpi_softc *, int, void *), void *, int);
void	acpi_enable_gpe(struct acpi_softc *, u_int32_t);

int	acpiec_intr(struct acpiec_softc *);
void	acpiec_read(struct acpiec_softc *, u_int8_t, int, u_int8_t *);
void	acpiec_write(struct acpiec_softc *, u_int8_t, int, u_int8_t *);
void	acpiec_handle_events(struct acpiec_softc *);

#if NACPIPWRRES > 0
int	acpipwrres_ref_incr(struct acpipwrres_softc *, struct aml_node *);
int	acpipwrres_ref_decr(struct acpipwrres_softc *, struct aml_node *);
#endif /* NACPIPWRRES > 0 */

int	acpi_read_pmreg(struct acpi_softc *, int, int);
void	acpi_write_pmreg(struct acpi_softc *, int, int, int);

void	acpi_poll(void *);
void	acpi_sleep(int, char *);

int	acpi_matchhids(struct acpi_attach_args *, const char *[], const char *);
int	acpi_parsehid(struct aml_node *, void *, char *, char *, size_t);

int	acpi_record_event(struct acpi_softc *, u_int);

void	acpi_addtask(struct acpi_softc *, void (*)(void *, int), void *, int);
int	acpi_dotask(struct acpi_softc *);

void	acpi_powerdown_task(void *, int);
void	acpi_sleep_task(void *, int);

/* Section 5.2.10.1: global lock acquire/release functions */
#define	GL_BIT_PENDING	0x01
#define	GL_BIT_OWNED	0x02
int	acpi_acquire_glk(uint32_t *);
int	acpi_release_glk(uint32_t *);

void	acpi_pciroots_attach(struct device *, void *, cfprint_t);

void	*acpi_intr_establish(int, int, int, int (*)(void *), void *,
	    const char *);

struct acpi_q *acpi_maptable(struct acpi_softc *sc, paddr_t,
	    const char *, const char *, const char *, int);

#endif

#endif /* !_ACPI_WAKECODE */
#endif	/* !_DEV_ACPI_ACPIVAR_H_ */
