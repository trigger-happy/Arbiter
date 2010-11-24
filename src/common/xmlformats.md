# XML Formats Documentation

In lieu of an XML Schema, this documentation is presented.

## Language Definition

### Example
	<language name="C++">
		<make>g++ @{filename} -o @{filename:base}</make>
		<run>./@{filename:base}</run>
		<platform list="win32-msvc win64-msvc">
			<make>cl @{filename}</make>
			<run>@{filename:base}.exe</run>
		</platform>
	</language>


## Problem Definition
All problem sets should contain a manifest.xml file which describes the files being used for the problem as well as run limits and problem metadata. The contents of the manifest are described below.

### Tag list
The following tags should appear in the manifest.

1. `<problem>` [root] - The root tag of the manifest, any other root tag will be flagged as an error. This tag has the following attributes:
	* `id` [required] - A unique string identifying this problem. For official ACM contests, the following format is `ACM_(NAT|RGN|FNL)_[LOC]_[YYYY].[MM]_[problem #]` where `NAT` = nationals, `RGN` = regionals, `FNL` = finals and `[LOC]` is the period-separated location of the contest. Problem numbers start with 1.
For example, `ACM_NAT_naga.ph_2010.11_1`
	* `title` [required] - Title of the problem.
2. `<brief>` [optional] - A short description of the problem.
3. `<author>` [optional] - The author's full name.
4. `<time_limit>` [optional] - Maximum time limit, in *seconds* that the run can execute *per case* (i.e. not cumulative).
5. `<mem_limit>` [optional] - Maximum memory, in *bytes* that the run can consume.
6. `<case>` [required] - Root tag for each case in the problem set.
	* `<input>` [optional] - Specifies the input file.
		* `as` [optional] - Signals that the run expects a file input thus the runner should copy the input file over as the specified filename. If blank or not specified, the runner assumes that the run will read from stdin so no copying is needed.
	* `<expected>` [required] - Specifies the output file which the run's output will be compared against.
		* `output` [optional] - Signals that the run will output to a file specified by the attributed. If blank or not specified, the runner defaults to stdout.

### Example
	<problem id="ACM_RGN_mnl.ph.apac_2" title="Sum of Squares">
		<brief>Compute the sum of squares</brief>
		<time_limit>120</time_limit>
		<mem_limit>1024</mem_limit>
		<author>Author Name</author>
		<case>
			<input as="f.in">1.in</input>
			<expected output="f.out">1.out</expected>
		</case>
		<case>
			<input>2.in</input>
			<expected>2.out</expected>
		</case>
	</problem>
