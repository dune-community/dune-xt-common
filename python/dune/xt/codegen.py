
def typeid_to_typedef_name(typeid, replacement='_'):
    """returns a sanitized typeid
    """
    illegal_chars = ['-', '>', '<', ':', ' ', ',', '+', '.']
    for ch in illegal_chars:
        typeid = typeid.replace(ch, replacement)
    return typeid