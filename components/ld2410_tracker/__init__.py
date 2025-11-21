import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

CONF_UART_ID = "uart_id"

# Callback names
CONF_UPDATE_OBJECT_PRESENT = "update_object_present"
CONF_UPDATE_ANGLE = "update_angle"
CONF_UPDATE_DISTANCE = "update_distance"
CONF_MOVE_STEPPER = "move_stepper"

ld2410_ns = cg.esphome_ns.namespace("ld2410_tracker")
LD2410Tracker = ld2410_ns.class_("LD2410Tracker", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(LD2410Tracker),

        cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),

        cv.Optional(CONF_UPDATE_OBJECT_PRESENT): cv.returning_lambda,
        cv.Optional(CONF_UPDATE_ANGLE): cv.returning_lambda,
        cv.Optional(CONF_UPDATE_DISTANCE): cv.returning_lambda,
        cv.Optional(CONF_MOVE_STEPPER): cv.returning_lambda,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    uart_component = await cg.get_variable(config[CONF_UART_ID])
    cg.add(var.set_uart(uart_component))

    # Bind callbacks
    if CONF_UPDATE_OBJECT_PRESENT in config:
        lambda_fn = await cg.process_lambda(
            config[CONF_UPDATE_OBJECT_PRESENT],
            args=[cg.bool_]
        )
        cg.add(var.set_update_object_present(lambda_fn))

    if CONF_UPDATE_ANGLE in config:
        lambda_fn = await cg.process_lambda(
            config[CONF_UPDATE_ANGLE],
            args=[cg.float_]
        )
        cg.add(var.set_update_angle(lambda_fn))

    if CONF_UPDATE_DISTANCE in config:
        lambda_fn = await cg.process_lambda(
            config[CONF_UPDATE_DISTANCE],
            args=[cg.float_]
        )
        cg.add(var.set_update_distance(lambda_fn))

    if CONF_MOVE_STEPPER in config:
        lambda_fn = await cg.process_lambda(
            config[CONF_MOVE_STEPPER],
            args=[cg.float_]
        )
        cg.add(var.set_move_stepper(lambda_fn))

    await cg.register_component(var, config)


