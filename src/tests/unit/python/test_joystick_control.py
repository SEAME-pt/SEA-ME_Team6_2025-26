#!/usr/bin/env python3
import sys
import types
import importlib
import struct
import io
from contextlib import redirect_stdout

import pytest



def make_fake_can():
    """Create a minimal fake 'can' module and insert into sys.modules.
    This prevents tests from needing the real python-can or hardware.
    """
    fake = types.ModuleType('can')

    class FakeMessage:
        def __init__(self, arbitration_id=None, data=None, is_extended_id=False):
            self.arbitration_id = arbitration_id
            self.data = data
            self.is_extended_id = is_extended_id

    class FakeBus:
        def __init__(self, *args, **kwargs):
            # record sent messages
            self.sent = []

        def send(self, msg):
            self.sent.append(msg)

        def shutdown(self):
            pass

    fake.interface = types.SimpleNamespace(Bus=FakeBus)
    fake.Message = FakeMessage

    class CanError(Exception):
        pass

    fake.CanError = CanError

    sys.modules['can'] = fake
    return fake


def import_module_with_fake_can():
    # Ensure we import a fresh copy of the module with our fake 'can'.
    if 'joystick_control' in sys.modules:
        del sys.modules['joystick_control']
    fake_can = make_fake_can()
    import joystick_control as jc
    importlib.reload(jc)
    return jc, fake_can



# verify CAN ID 
def test_send_command_clamps_and_sends():
    jc, fake_can = import_module_with_fake_can()

    ctrl = jc.VehicleController(can_interface='can1')

    # call with out-of-range values (should be clamped to [-100,100])
    ok = ctrl.send_command(200, -200)
    assert ok is True

    # the FakeBus records sent messages in .sent
    assert hasattr(ctrl.can_bus, 'sent')
    msg = ctrl.can_bus.sent[-1]
    assert msg.arbitration_id == jc.CAN_ID_JOYSTICK
    assert msg.data == struct.pack('<hh', 100, -100)

# error handling
def test_send_command_handles_can_error():
    jc, fake_can = import_module_with_fake_can()

    ctrl = jc.VehicleController(can_interface='can1')

    # Replace the send method to raise the module's CanError
    def raise_err(msg):
        raise fake_can.CanError('boom')

    ctrl.can_bus.send = raise_err

    ok = ctrl.send_command(10, 20)
    assert ok is False

# tests stop calls
def test_stop_calls_send_command_and_prints():
    jc, fake_can = import_module_with_fake_can()

    ctrl = jc.VehicleController(can_interface='can1')

    calls = []

    def recorder(s, t):
        calls.append((s, t))
        return True

    # monkeypatch the instance method
    ctrl.send_command = recorder

    f = io.StringIO()
    with redirect_stdout(f):
        ctrl.stop()
    out = f.getvalue()

    assert (0, 0) in calls
    assert 'STOP enviado' in out

# ensures emergency stop works
def test_modo_emergency_stop_sends_three_times():
    jc, _ = import_module_with_fake_can()

    class Dummy:
        def __init__(self):
            self.calls = []

        def send_command(self, s, t):
            self.calls.append((s, t))

    dummy = Dummy()

    # speed up by disabling sleep
    orig_sleep = jc.time.sleep
    jc.time.sleep = lambda x: None
    try:
        jc.modo_emergency_stop(dummy)
    finally:
        jc.time.sleep = orig_sleep

    assert len(dummy.calls) == 3
    assert all(c == (0, 0) for c in dummy.calls)


def _contains_subsequence(lst, subseq):
    idx = 0
    for x in lst:
        if x == subseq[idx]:
            idx += 1
            if idx == len(subseq):
                return True
    return False


# tests steering and throttle
def test_modo_testes_calls_expected_sequences():
    jc, _ = import_module_with_fake_can()

    class Dummy:
        def __init__(self):
            self.calls = []

        def send_command(self, s, t):
            # record numeric commands only
            self.calls.append((s, t))

        def print_status(self, s, t):
            pass

        def stop(self):
            self.calls.append(('stopped',))

    dummy = Dummy()

    # disable sleeps to make the test fast
    orig_sleep = jc.time.sleep
    jc.time.sleep = lambda x: None
    try:
        jc.modo_testes(dummy)
    finally:
        jc.time.sleep = orig_sleep

    # filter only the (steering, throttle) tuples
    tuple_calls = [c for c in dummy.calls if isinstance(c, tuple) and len(c) == 2]

    expected_steer = [(-100, 0), (-50, 0), (0, 0), (50, 0), (100, 0), (0, 0)]
    expected_throt = [(0, 25), (0, 50), (0, 0), (0, -25), (0, 0)]

    assert _contains_subsequence(tuple_calls, expected_steer)
    assert _contains_subsequence(tuple_calls, expected_throt)
