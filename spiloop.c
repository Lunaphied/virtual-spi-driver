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
	if (!xfer) {
		return -EIO;
		spi_finalize_current_transfer(conn);
	}

	if (xfer->rx_buf && (xfer->len >= 1)) {
		uint8_t *rx = (uint8_t*) xfer->rx_buf;
		rx[0] = 'a';
	}
	spi_finalize_current_transfer(conn);
	return 0;
}

static struct spi_board_info fake_device_info =
{
	.modalias = "spidev",
	.max_speed_hz = 1,
	.bus_num = 0,
	.chip_select = 0,
	.mode = SPI_MODE_0
};

static int __init controller_init(void) {
	int status;
	struct spi_device *dev;
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
	dev = spi_new_device(conn, &fake_device_info);
	if (!dev) {
		status = -1;
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
