import time
import logging

class Microcontroller:

    def __init__(self, itf, **kwargs):
        """
        Python module running on server side. Talk to microcontroller
        through serial port.
            itf     serial port, instance of pyserial
        """
        self._itf = itf
        self.logger = kwargs.get('logger', logging.getLogger(__name__))
        self.logger.setLevel(kwargs.get('loggerlevel', logging.WARNING))

        self.logger.info('Interface info: {}'.format(self._itf.name))

        if not self.is_alive():
            logger.error('Cannot reach to microcontroller')

    def _write(self, cmd):
        assert isinstance(cmd, str)
        self._itf.write(cmd)

    def _read(self):
        return self._itf.read_all()

    def is_alive(self):
        ret = self.get('help')
        return 'help' in ret

    def exit(self):
        """
        Close serial port and itself, does not shutdown the microcontroller
        """
        self._itf.close()

    def set(self, cmd):
        self._write(cmd)
        time.sleep(2)

    def get(self, cmd):
        self._write(cmd)
        time.sleep(2)
        return self._read()

