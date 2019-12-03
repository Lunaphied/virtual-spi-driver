#include <linux/spi/spi.h>
#include <linux/module.h>
#include <linux/miscdevice.h>

struct miscdevice fake_controller = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fake_spicon"
};

static struct spi_controller *conn;


static int virtual_spi_one_transfer(struct spi_master *master,
					struct spi_device *spi,
					struct spi_transfer *xfer)
{
	spi_finalize_current_transfer(conn);
	return 0;
}

static int __init controller_init(void) {
	int status;
	status = misc_register(&fake_controller);
	if (status) {
		return status;
	}
	conn = spi_alloc_master(fake_controller.this_device, 0);
	if (!conn) {
		status = -1;
		goto out_free;
	}
	conn->bus_num = 0;
	conn->num_chipselect = 1;
	conn->transfer_one = virtual_spi_one_transfer;
	status = spi_register_controller(conn);
	if (status) {
		goto out_free;
	}
	return status;
out_free:
	misc_deregister(&fake_controller);
	spi_master_put(conn);
	return status;
}

static void __exit controller_exit(void) {
	if (conn) {
		spi_unregister_controller(conn);
	}
	misc_deregister(&fake_controller);
}

module_init(controller_init);
module_exit(controller_exit);

MODULE_LICENSE("GPL");
