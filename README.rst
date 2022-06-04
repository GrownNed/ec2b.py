ec2b.py
=======

Installing
----------

.. code:: sh

    git clone https://github.com/GrownNed/ec2b.py
    cd ec2b.py
    pip install .

Example
-------

.. code:: py

    import ec2b

    with open("ec2b_seed.bin", "rb") as file:
        seed = file.read()

    key = ec2b.derive(seed)

Links
-----
- `Original Hotaru's C/C++ code <https://github.com/HotaruYS/Ec2b>`_
