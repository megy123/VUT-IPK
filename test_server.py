##############
# IPK stream #
# 2024-03-08 #
##############

#
# author: okurka12 (https://github.com/okurka12)
#
# python version: 3.12.2
#
# see README.md for more information
# https://github.com/okurka12/ipk_proj1_livestream/blob/main/README.md
#

# inspiration
# https://wiki.python.org/moin/UdpCommunication

import socket
from random import randint
from typing import Union
from time import sleep

# what address to listen on
# BIND_IP = "127.0.0.1"  # localhost (loopback only)
BIND_IP = "0.0.0.0"  # listen on every available network interface
UDP_PORT = 4567  # default IPK24-CHAT port

# timeout for recv-loop
RECV_TIMEOUT = 0.01

# if this flag is True, the server responds from a dynamic port, else,
# it sends all responses from the default port
# recommended value: True
REPLY_FROM_DYNAMIC_PORT = True

# if this flag is True, server sends a CONFIRM message to AUTH message from
# from the default port whether or not REPLY_FROM_DYNAMIC_PORT is True or not
# note: it confirms _every_ AUTH message from the default port
# recommended value: True
CONFIRM_AUTH_FROM_DEFAULT_PORT = True

# if true, always respond to AUTH and JOIN messages with REPLY
# where result=1 (success) else, respond with REPLY where result=0 (failure)
# recommended value: True
REPLY_SUCCESS = True

# send this many MSG replies to MSG messages from client
# setting this to more than 1 can enable you to test what your client
# does when the same message (with the same id) comes more than once
# recommended value: 1
MSG_REPLY_COUNT: int = 1

# in MSG replies to MSG messages from client, contain this many characters
# of the client's message contents
# recommended value: 15
CONTENT_PREFIX_LENGTH: int = 15

# delay in seconds between sending a reply MSG and a reply BYE to a message
# from client that contains the word 'bye'
# recommended value: 0.05
MSG_BYE_DELAY: float = 0.05

FAMILY = socket.AF_INET

# do we want to prin bloat?
VERBOSE = False

# https://stackoverflow.com/questions/5815675/what-is-sock-dgram-and-sock-stream
TYPE = socket.SOCK_DGRAM

# dictionary that maps message type byte to the string representation
# eg. 0x00 -> "CONFIRM"
MSG_TYPES: dict[str] = {
    0x00: "CONFIRM",
    0x01: "REPLY",
    0x02: "AUTH",
    0x03: "JOIN",
    0x04: "MSG",
    0xFE: "ERR",
    0xFF: "BYE",
}

# dictionary that maps message types from string to binary
# eg. "CONFIRM" -> 0x00
MSG_INV_TYPES = {value: key for key, value in MSG_TYPES.items()}


class Message:
    def __init__(self, msg: bytes) -> None:
        # parse message header
        self.type = int(msg[0])
        self.type = \
            MSG_TYPES[self.type] if self.type in MSG_TYPES else "unknown"
        self.id = int.from_bytes(msg[1:3], byteorder="big")

        # binary form
        self.binary = msg

        # message type
        msgt = self.type

        if   msgt == "CONFIRM":
            self.ref_msgid = self.id
            del self.id
        elif msgt == "REPLY":
            self.result = int(msg[3])
            self.ref_msgid = int.from_bytes(msg[4:6], byteorder="big")
            self.content = str_from_bytes(6, msg)
        elif msgt == "AUTH":
            self.username = str_from_bytes(3, msg)
            self.dname = str_from_bytes(3 + len(self.username) + 1, msg)
            self.secret = str_from_bytes(3 + len(self.username) + 1 +
                                         len(self.dname) + 1, msg)
        elif msgt == "JOIN":
            self.chid = str_from_bytes(3, msg)
            self.dname = str_from_bytes(3 + len(self.chid) + 1, msg)
        elif msgt == "MSG" or msgt == "ERR":
            self.dname = str_from_bytes(3, msg)
            self.content = str_from_bytes(3 + len(self.dname) + 1, msg)
        elif msgt == "BYE":
            pass

    def __repr__(self) -> str:

        delim: str = "\n"
        output_list = []

        if self.type is not None:
            output_list.append(f"TYPE: {self.type}")
        if self.id is not None:
            output_list.append(f"ID: {self.id}")
        if self.ref_msgid is not None:
            output_list.append(f"REF ID: {self.ref_msgid}")
        if self.username is not None:
            output_list.append(f"USERNAME: '{no_lf(self.username)}'")
        if self.dname is not None:
            output_list.append(f"DISPLAY NAME: '{no_lf(self.dname)}'")
        if self.secret is not None:
            output_list.append(f"SECRET: '{no_lf(self.secret)}'")
        if self.chid is not None:
            output_list.append(f"CHANNEL ID: '{no_lf(self.chid)}'")
        if self.result is not None:
            output_list.append(f"RESULT: {self.result}")
        if self.content is not None:
            output_list.append(f"'{no_lf(self.content)}'")

        output_list.append(str(self.binary))

        return delim.join(output_list)

    def __getattr__(self, attr) -> None:
        if VERBOSE:
            print(f"ERROR: MSG:{hex(id(self))} has no attribute '{attr}' "
                f"(mtype: {self.type})")


def str_from_bytes(startpos: int, b: bytes) -> str:
    """
    reads a null terminated string beginning at `startpos` in `b`.
    returns the string
    o k u r k a 1 2
    """
    output = ""
    i = startpos
    while b[i] != 0 and i < len(b) - 1:
        output += chr(b[i])
        i += 1
    return output


def sitb(i: int) -> bytearray:
    """convert a short int (0 to 65535) to big-endian bytearray"""
    assert 0 <= i <= 65535
    msb = i // 0x0100
    lsb = i % 0x0100
    return bytearray([msb, lsb])



def no_lf(s: str) -> str:
    r"""replaces (CR)LF with \n (like actual backslash and letter n)"""
    o = s.replace("\r", "")
    return o.replace("\n", "\\n")


def render_msg(dname: str, content: str, id: int) -> bytearray:
    assert all(0x21 <= ord(c) <= 0x7e for c in dname)
    assert all(0x20 <= ord(c) <= 0x7e for c in content)
    assert 1 <= len(dname) <= 20
    assert 1 <= len(content) <= 1400
    output = bytearray()
    output.append(MSG_INV_TYPES["MSG"])       # 0x04 - MSG
    output.extend(sitb(id))                   # message id
    output.extend([ord(c) for c in dname])    # dname
    output.append(0)                          # zero byte
    output.extend([ord(c) for c in content])  # message contents
    output.append(0)                          # zero byte
    return output


def render_bye(id: int) -> bytearray:
    output = bytearray()
    output.append(MSG_INV_TYPES["BYE"])
    output.extend(sitb(id))
    return output

def render_confirm(msg: Message) -> bytearray:
    assert msg.type != "CONFIRM"  # dont confirm confirms
    output = bytearray()
    output.append(MSG_INV_TYPES["CONFIRM"])  # 0x00
    output.extend(sitb(msg.id))              # ref. message id
    return output


def render_reply(msg: Message, id: int, succ: bool, cont: str) -> bytearray:
    """
    render a REPLY message to `msg` with id `id`, message content `cont`
    and result according to `succ` (success, `succ` being True
    means result=1)
    """
    assert (len(cont) <= 1400)
    output = bytearray()
    output.append(MSG_INV_TYPES["REPLY"])    # 0x01
    output.extend(sitb(id))                  # message id
    output.append(1 if succ else 0)          # result
    output.extend(sitb(msg.id))              # ref. message id
    output.extend([ord(c) for c in cont])    # message content
    output.append(0)                         # zero byte
    return output

def create_reply_text_msg(msg: Message) -> str:
    """
    creates a text for the MSG message from client, tries to contain
    all the message information in the reply text (id, display name,
    message content)

    note it supposes that the word 'bye' in the client's message means
    the server will send it a BYE message
    """
    assert msg.type == "MSG"
    reply_text = f"Hi, {msg.dname}! This is a reply MSG to your MSG " \
        f"id={msg.id} content='{msg.content[:CONTENT_PREFIX_LENGTH]}...' :)"
    if "bye" in msg.content:
        reply_text += " since the word 'bye' was in your message, the " \
            "server will also send you a BYE message."
    return reply_text

def create_reply_text_reply(msg: Message, succ: bool) -> str:
    """
    same as `create_reply_text_msg`, but for AUTH and JOIN
    `succ` parameter has the same semantic as in `render_reply`
    """
    assert msg.type == "AUTH" or msg.type == "JOIN"
    reply_text = f"Hi, {msg.dname}, this is "
    reply_text += "a successful " if succ else \
        "an unsuccessful "
    reply_text += f"REPLY message to your {msg.type} message id={msg.id}. "
    if msg.type == "AUTH":
        reply_text += f"You wanted to authenticate under the username "
        reply_text += msg.username
    elif msg.type == "JOIN":
        reply_text += f"You wanted to join the channel {msg.chid}"
    return reply_text


def print_message(msg: Message, addr: tuple[str, int], port: Union[int, str]):
    """
    print that a message came from `addr` to port `port`
    and the message itself
    """
    print(f"Message from {addr[0]}:{addr[1]} came to port {port}:")
    print(msg)


def confirm_message(dynportsock, defportsock, retaddr, msg: Message) -> None:
    """
    confirms a message `msg` either from `dynportsock` or `defportsoct`
    according to `REPLY_FROM_DYNAMIC_PORT` and
    `CONFIRM_AUTH_FROM_DEFAULT_PORT` constants
    """
    print(f"Confirming {msg.type} message id={msg.id}")
    reply = render_confirm(msg)
    if CONFIRM_AUTH_FROM_DEFAULT_PORT and msg.type == "AUTH":
        defportsock.sendto(reply, retaddr)
    else:
        if REPLY_FROM_DYNAMIC_PORT:
            dynportsock.sendto(reply, retaddr)
        else:
            defportsock.sendto(reply, retaddr)


def send_response(sock, retaddr, msg: Message) -> None:
    """
    according to `msg`'s type, creates and sends a response to it,
    that can be either MSG, REPLY or BYE message
    """
    if msg.type == "AUTH" or msg.type == "JOIN":
        print(f"sending REPLY with result={1 if REPLY_SUCCESS else 0} "
                f"to {msg.type} msg id={msg.id}")
        reply_text = create_reply_text_reply(msg, REPLY_SUCCESS)
        reply = render_reply(msg, randint(0, 0xffff), REPLY_SUCCESS,
                                reply_text)
        sock.sendto(reply, retaddr)

    if msg.type == "MSG":
        reply_text = create_reply_text_msg(msg)
        reply = render_msg("Server", reply_text, randint(0, 0xffff))
        for _ in range(MSG_REPLY_COUNT):  # send n times
            sock.sendto(reply, retaddr)

    if msg.type == "MSG" and "bye" in msg.content:
        sleep(MSG_BYE_DELAY)
        print("sending BYE...")
        reply = render_bye(0xffff)
        sock.sendto(reply, retaddr)


def recv_loop(sock: socket.socket) -> None:

    # socket to send replies from
    sock_dynport = socket.socket(FAMILY, TYPE)
    sock_dynport.settimeout(RECV_TIMEOUT)

    reply_socket = sock_dynport if REPLY_FROM_DYNAMIC_PORT else sock

    # basically active waiting, the timeout on the sockets is really small
    while True:
        came_to_def_port = False
        came_to_dyn_port = False

        # wait for the message
        try:
            response, retaddr = sock.recvfrom(2048)
            came_to_def_port = True
        except TimeoutError:
            pass
        if not came_to_def_port:
            try:
                response, retaddr = sock_dynport.recvfrom(2048)
                came_to_dyn_port = True
            except TimeoutError:
                pass

        # no message came, go to the start of the loop
        if not came_to_def_port and not came_to_dyn_port:
            continue

        # message came, parse it
        msg = Message(response)

        # print on stdout
        if msg.type != "CONFIRM":
            print()  # blank line
            from_port = "dyn2" if came_to_dyn_port else UDP_PORT
            print_message(msg, retaddr, from_port)

        # confirm
        if msg.type != "CONFIRM":
            confirm_message(sock_dynport, sock, retaddr, msg)

        # reply
        send_response(reply_socket, retaddr, msg)


def main():

    # create socket and bind
    sock = socket.socket(FAMILY, TYPE)
    sock.bind((BIND_IP, UDP_PORT))
    sock.settimeout(RECV_TIMEOUT)
    print(f"started server on {BIND_IP} port {UDP_PORT}")

    try:
        recv_loop(sock)
    except KeyboardInterrupt:
        print()


    print("exiting...")


if __name__ == "__main__":
    main()