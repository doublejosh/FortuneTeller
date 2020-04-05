import React, { useMemo } from 'react'
import { css } from 'glamor'
import { useSelector } from 'react-redux'
//import BarChart from 'react-d3-components'

export default props => {
	const theme = useSelector(state => state.theme) || []

	let totals = { votes: 1, shown: 1 }
	const findTotal = fortunes => {
		let votes = 0,
			shown = 0
		if (fortunes) {
			fortunes.forEach(f => {
				votes += f.votes
				shown += f.shown
			})
		}
		return { votes: votes, shown: shown }
	}
	totals = useMemo(() => findTotal(props.fortunes), [props.fortunes])

	// var data = [
	// 	{
	// 		label: 'somethingA',
	// 		values: [
	// 			{ x: 'SomethingA', y: 10 },
	// 			{ x: 'SomethingB', y: 4 },
	// 			{ x: 'SomethingC', y: 3 },
	// 		],
	// 	},
	// ]

	return (
		<section
			{...css({
				fontSize: '2.5rem',
				margin: '0 0 24rem 0',
				color: 'rgba(0, 0, 0, 0.6)',
				textAlign: 'center',
				[`@media(min-width: ${theme.breaks.md}px)`]: {
					textAlign: 'left',
					margin: '0 0 6rem 1rem',
				},
			})}>
			<span {...css({ fontSize: '7rem' })}>
				{totals.votes && totals.shown
					? Math.round((parseInt(totals.votes) / parseInt(totals.shown)) * 100)
					: '??'}
				%
			</span>
			<br />
			accurate <br />({totals.votes}/{totals.shown})
			{/* <BarChart
				data={data}
				width={400}
				height={400}
				margin={{ top: 10, bottom: 50, left: 50, right: 10 }}
			/> */}
		</section>
	)
}
