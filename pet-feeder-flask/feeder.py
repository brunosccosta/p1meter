import tinytuya
import logging

class Feeder:
    client: tinytuya.Device

    DPS_FOOD = 3  # Constant for the food DPS (Data Point Index)
    DPS_LIGHT = 19  # Constant for the light DPS (Data Point Index)

    def __init__(self, ip: str, dev_id: str, local_key: str):
        logging.info(f"Connecting to feeder @ {ip}")

        self.client = tinytuya.Device(
            dev_id=dev_id,
            address=ip,
            local_key=local_key,
            version=3.3)

    def give_food(self, amount=1):
        self.client.set_value(self.DPS_FOOD, amount, nowait=False)

    def set_light(self, state):
        self.client.set_value(self.DPS_LIGHT, state, nowait=False)

    def get_status(self):
        data = self.client.status()

        return {
            'manual_feed': data['dps']['3'],
            'feed_state': data['dps']['4'],
            'light': data['dps']['19']
        }