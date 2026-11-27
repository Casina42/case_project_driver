#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/kernel_stat.h>
#include <linux/cpumask.h>
#include <linux/fs.h>
#include <linux/byteorder/generic.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fede Casina");
MODULE_DESCRIPTION("Invia utilizzo CPU (64-bit) ad Arduino");
MODULE_VERSION("0.2");

// Percorso della porta seriale di Arduino (controlla con 'ls /dev/tty*')
#define ARDUINO_PORT "/dev/arduino_cpu"	//note: see 'notes/device_recognition.md'

static struct delayed_work cpu_work;
static struct file *arduino_filp = NULL;

static void send_cpu_usage_to_device(int usage)
{
	u64 packet;
	loff_t pos = 0;
	ssize_t ret;

	// Se la porta non è aperta, prova ad aprirla (utile se colleghi Arduino dopo insmod)
	if (IS_ERR_OR_NULL(arduino_filp)) {
		arduino_filp = filp_open(ARDUINO_PORT, O_WRONLY | O_NOCTTY, 0);
		if (IS_ERR(arduino_filp)) {
			pr_warn_ratelimited("Miodriver: impossibile aprire %s (err: %ld)\n", 
						ARDUINO_PORT, PTR_ERR(arduino_filp));
			arduino_filp = NULL;
			return;
		}
		pr_info("Miodriver: connesso ad Arduino su %s\n", ARDUINO_PORT);
	}

	// Costruisce il pacchetto a 64 bit (es. metti il valore nei primi byte, 
	// o aggiungi un header negli altri bit per identificare il pacchetto)
	packet = cpu_to_le64((u64)usage);

	// Invia esattamente 8 byte (64 bit) al dispositivo
	ret = kernel_write(arduino_filp, &packet, sizeof(packet), &pos);
	
	if (ret < 0) {
		pr_err("Miodriver: errore scrittura su Arduino (%zd), chiudo connessione.\n", ret);
		filp_close(arduino_filp, NULL);
		arduino_filp = NULL;
	} else {
		pr_info("Miodriver: inviati %zd byte (CPU: %d%%)\n", ret, usage);
	}
}

// Nota: il parametro DEVE essere un puntatore (*work)
static void cpu_work_fn(struct work_struct *work)
{
	static u64 prev_idle = 0;
	static u64 prev_total = 0;
	u64 idle, total;
	u64 idle_delta, total_delta;
	int cpu_usage = 0;
	int cpu = 0;

	total = kcpustat_cpu(cpu).cpustat[CPUTIME_USER] +
			kcpustat_cpu(cpu).cpustat[CPUTIME_SYSTEM] +
			kcpustat_cpu(cpu).cpustat[CPUTIME_IDLE];
	idle = kcpustat_cpu(cpu).cpustat[CPUTIME_IDLE];

	idle_delta = idle - prev_idle;
	total_delta = total - prev_total;
	
	if (total_delta > 0)
		cpu_usage = 100 - (idle_delta * 100 / total_delta);

	prev_idle = idle;
	prev_total = total;

	send_cpu_usage_to_device(cpu_usage);

	// Riprogramma l'esecuzione tra 1 secondo
	schedule_delayed_work(&cpu_work, msecs_to_jiffies(1000));
}

static int __init miodriver_init(void)
{
	printk(KERN_INFO "Miodriver caricato con successo!\n");

	// Inizializza e avvia la workqueue
	INIT_DELAYED_WORK(&cpu_work, cpu_work_fn);
	schedule_delayed_work(&cpu_work, msecs_to_jiffies(1000));

	return 0;
}

static void __exit miodriver_exit(void)
{
	// Ferma la workqueue in modo sicuro prima di scaricare il modulo
	cancel_delayed_work_sync(&cpu_work);

	// Chiude il file verso Arduino se aperto
	if (!IS_ERR_OR_NULL(arduino_filp)) {
		filp_close(arduino_filp, NULL);
	}

	printk(KERN_INFO "Miodriver scaricato.\n");
}

module_init(miodriver_init);
module_exit(miodriver_exit);