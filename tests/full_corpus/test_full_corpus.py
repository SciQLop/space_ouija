from space_ouija.ouija_boards.cassini._cassini import load_RPWS_LOW_RATE_FULL_MFR0, SENSOR_NUMBER, load_RPWS_WBR_WFR, load_RPWS_WFR
import requests
import os
from tempfile import TemporaryDirectory

files = [
 #"https://pds-ppi.igpp.ucla.edu/data/CO-V_E_J_S_SS-RPWS-2-REFDR-WFRFULL-V1.0/DATA/RPWS_WAVEFORM_FULL/T20143XX/T2014300/T2014300_25HZ1_WFRFR.DAT",
 "https://pds-ppi.igpp.ucla.edu/data/CO-V_E_J_S_SS-RPWS-2-REFDR-WFRFULL-V1.0/DATA/RPWS_WAVEFORM_FULL/T20061XX/T2006102/T2006102_25HZ1_WFRFR.DAT",
 ]


dest_dir = TemporaryDirectory()

for url in files:
    filename = url.split("/")[-1]
    dest_file = os.path.join(dest_dir.name, filename)
    if not os.path.exists(dest_file):
        with open(dest_file, "wb") as f:
            f.write(requests.get(url).content)
        if "WFRFR" in filename:
            data = load_RPWS_WFR(dest_file)
