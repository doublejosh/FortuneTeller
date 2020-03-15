import React, { useMemo } from 'react'
import { css } from 'glamor'
import Chart from './Chart'

let style = css({
	fontSize: '2.5rem',
	margin: '0 0 24rem 0',
	color: 'rgba(0, 0, 0, 0.6)',
	textAlign: 'center',
	'@media (min-width: 600px)': {
		textAlign: 'left',
		margin: '0 0 6rem 0',
	},
})

export default ({ fortunes = [] }) => {
	let totals = { votes: 1, shown: 1 }

	const findTotal = fortunes => {
		let votes = 0, shown = 0
		if (fortunes) {
			fortunes.forEach(f => {
				votes += f.votes
				shown += f.shown
				f.score = votes / shown
			})
		}
		return { votes: votes, shown: shown }
	}

	// Remove empty items from Firebase.
	fortunes = useMemo(() => fortunes.filter(obj => obj), [fortunes])
	totals = useMemo(() => findTotal(fortunes), [fortunes])

	return (
		<section className={style}>
			<Chart fortunes={fortunes} />
			{Math.round(
				(parseInt(totals.votes) / parseInt(totals.shown)) * 100
			)}
			% accurate <br />({totals.votes}/{totals.shown})
		</section>
	)
}
