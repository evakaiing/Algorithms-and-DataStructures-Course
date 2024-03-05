import os
import argparse
import sys


def get_arguments():
    parser = argparse.ArgumentParser(
            description='Concatenate the specified file a specified number of times so that the length '
                        'of the resulting file in bytes does not exceed the specified number.')
    parser.add_argument('FILE', type=str, help='File for concatenation')
    parser.add_argument('REPETITION', type=int, help='Number of file concatenations')
    parser.add_argument('MAXSIZE', type=int, help='The maximum size of the resulting file')
    return parser.parse_args()


def error_processing(args):
    if os.path.getsize(args.FILE) == 0:
        sys.exit(-1)


def concatenate(args):
    error_processing(args)
    with open(args.FILE, 'r', encoding="UTF-8") as file:
        in_file = file.read()

    current_size = os.path.getsize(args.FILE)
    while current_size + len(in_file) * args.REPETITION <= args.MAXSIZE:
        with open(args.FILE, 'a', encoding="UTF-8") as file:
            file.write(in_file)
        current_size = os.path.getsize(args.FILE)


def main():
    args = get_arguments()
    concatenate(args)


if __name__ == '__main__':
    main()
