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
        in_file = file.readline()
        current_size = os.path.getsize(args.FILE)
        size = current_size

    with open(args.FILE, 'a', encoding="UTF-8") as file:
        for _ in range(args.REPETITION):
            if current_size + size <= args.MAXSIZE:
                file.write(in_file)
                current_size += size
            else:
                open(args.FILE, 'w').close()
                sys.exit(-1)


def main():
    args = get_arguments()
    concatenate(args)


if __name__ == '__main__':
    main()
