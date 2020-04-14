import React from 'react'
import { useSelector } from 'react-redux'
import { useFirebaseConnect, isEmpty } from 'react-redux-firebase'
import Fortune from '../Dashboard/Fortune'
import { css } from 'glamor'

// Vote Order...
// V1 - One at a time, pure vote recency.
// V2 -  List of lowest voted-on (newest first), client loop.

export default props => {
	useFirebaseConnect(['fortunes'])
	let fortunes = useSelector(state => state.firebase.data.fortunes)

	const list = Object.keys(fortunes || {})
		.filter(f => fortunes[f])
		.map(f => {
			return {
				...fortunes[f],
				key: f,
				rankVotes: fortunes[f].rank && fortunes[f].rank.total ? fortunes[f].rank.total : 0,
				rankLastVote:
					fortunes[f].rank && fortunes[f].rank.lastVote ? fortunes[f].rank.lastVote : 0,
				score: 20,
			}
		})
		.sort((a, b) => {
			if (a.key.toString() > b.key.toString()) return -1
			if (a.key.toString() < b.key.toString()) return 1
			return 0
		})
		.sort((a, b) => a.rankVotes - b.rankVotes)
		.sort((a, b) => a.rankLastVote - b.rankLastVote)

	const total = !isEmpty(list)
		? list.reduce((total, f) => (f.rank && f.rank.total ? f.rank.total + total : total), 0)
		: 0

	return (
		<section {...css({ margin: '1rem' })}>
			{fortunes ? (
				<React.Fragment>
					<h3
						{...css({
							margin: 0,
							fontSize: '1.25rem',
							color: 'rgba(0,0,0, .5)',
							textAlign: 'right',
							textTransform: 'uppercase',
						})}>
						Approval
					</h3>
					<ul {...css({ paddingLeft: '0', listStyle: 'none', marginTop: '.5rem' })}>
						{list.map((f, i) => (
							<Fortune key={i} f={f} hideVotes={true} />
						))}
					</ul>
					<div
						{...css({
							textAlign: 'right',
							marginTop: '1rem',
							fontSize: '3rem',
							lineHeight: '3rem',
							color: 'rgba(0, 0, 0, 0.6)',
						})}>
						<h2 {...css({ marginBottom: 0 })}>{total}</h2>
						<span>votes</span>
					</div>
				</React.Fragment>
			) : (
				<h2
					{...css({
						gridArea: 'message',
						display: 'flex',
						alignItems: 'center',
						justifyContent: 'center',
						animation: 'blinkingText 1.2s infinite',
					})}>
					Loading...
				</h2>
			)}
		</section>
	)
}
