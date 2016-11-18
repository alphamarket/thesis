#!/bin/bash
find ./ -name "*.fig" -type f -delete
find ./ -name "*.png" -exec convert {} -trim {} \;