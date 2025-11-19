import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

ld2410_ns = cg.esphome_ns.namespace("ld2410_tracker")
LD2410Tracker = ld2410_ns.class_("LD2410Tracker", cg.Component)

CONF_UART_ID = "uart_id"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(LD2410Tracker),
    cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),
})


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    uart_component = await cg.get_variable(config[CONF_UART_ID])
    cg.add(var.set_uart(uart_component))
    await cg.register_component(var, config)
