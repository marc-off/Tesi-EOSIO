<h1 class="contract">upsert</h1>
---
spec-version: 0.0.2
title: Upsert
summary: This action will either insert or update an entry in the authorities table. If an entry exists with the same key as the result of a hash function based on input owner-perm-account specified by owner parameter, the record is updated with the key, owner, perm, account and account perm parameters. If a record does not exist, a new record is created. The data is stored in the multi index table. The ram costs are paid by the authadmin.
icon:

<h1 class="contract">erase</h1>
---
spec-version: 0.0.2
title: Erase
summary: This action will remove an entry from the authorities table if an entry in the multi index table exists with the specified key as the result of a hash function based on input owner-perm-account.
icon: