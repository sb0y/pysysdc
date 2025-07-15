import json
from functools import lru_cache
from pysysdc.sdbus import SDClient
import logging

class KnockerClient(object):
        @staticmethod
        @lru_cache(maxsize=None)
        def get_sd():
                #sd = SDClient(service_name="dc.cloud.auth", path="/dc/cloud/auth", if_name="dc.cloud.auth", method_name="Token", method_args="s", method_return="is", output_sig="is")
                return SDClient(service_name="dc.cloud.status", path="/dc/cloud/status", if_name="dc.cloud.status", method_name="Report", method_args="s", method_return="i", output_sig="i")

        @staticmethod
        def knocker_status(status: dict) -> bool:
                #sd = KnockerClient.get_sd()
                sd = SDClient(service_name="dc.cloud.status", path="/dc/cloud/status", if_name="dc.cloud.status", method_name="Report", method_args="s", output_sig="i")
                st = '{"category": "CashRegister", "brand": "passport", "model": "passport", "state": "3", "recordedAt": "2023-12-28 10:51:50.876156"}'
                ret = sd.send(first_arg=st, output_sig="i") #json.dumps(status)
                #print(ret)
                return ret.py_code > 0 if True else False

        @staticmethod
        def instant_on_status(status: dict) -> bool:
                sd = KnockerClient.get_sd()
                st = '{"category": "CashRegister", "brand": "passport", "model": "passport", "state": "3", "recordedAt": "2023-12-28 10:51:50.876156"}'
                logging.warning("to sd %s", st)
                ret = sd.send(first_arg=st)  #json.dumps(status)
                return ret.py_code > 0 if True else False

#busctl --system call dc.cloud.status /dc/cloud/status dc.cloud.status Report s '{"category": "CashRegister", "state": "test", "model": "passport"}'
#busctl --system call dc.cloud.status /dc/cloud/status dc.cloud.status Report s '{"category": "CashRegister", "brand": "verifone", "model": "test", "state": "SERVICE_DOWN", "recordedAt": "2023-12-29 08:58:40.304769"}'
KnockerClient.knocker_status({})
