""" Miscelaneous utilities.
"""

def bytearray_to_packet_str(packet_ba: bytearray) -> str:
    """ Converts a bytearray to its hex text representation: '0x ...'
    """
    return "0x " + "".join(["%02X " % x for x in packet_ba])


def csv_header_from_packet(packet_type) -> str:
    """ Builds a CSV header from a packet type using get_fields() method.
    """
    return "".join( ["{},".format(f) for f in packet_type().get_fields()] )[:-1]    