"""Refactor observation table

Revision ID: c88293c7d72e
Revises: 22487ba09fe3
Create Date: 2021-02-08 16:11:55.761395

"""
from alembic import op
import sqlalchemy as sa
import sys

# revision identifiers, used by Alembic.
revision = "c88293c7d72e"
down_revision = "22487ba09fe3"
branch_labels = None
depends_on = None


def upgrade():
    # ### commands auto generated by Alembic - please adjust! ###
    with op.batch_alter_table("observation") as bop:
        bop.alter_column("key_indices", new_column_name="x_axis")
        bop.drop_column("data_indices")
    # ### end Alembic commands ###


def downgrade():
    # ### commands auto generated by Alembic - please adjust! ###
    sys.exit("Cannot downgrade")
    # ### end Alembic commands ###
